#include <modules/Renderer.hpp>

#include <core/Logger.hpp>

#include <core/Utils.hpp>

using namespace TLOT;

Renderer::Renderer ()
{
	m_vao .Bind ();
	m_vbo .Create ();
	m_ebo .Create ();
	m_dibo.Create ();

	m_ssboInstanceIndex.Create ();
	m_ssboInstanceData .Create ();
	m_ssboTextureOffset.Create ();
}

GeometryID Renderer::RegisterGeometry (Mesh const & mesh)
{
	if (m_registeredGeometry.find (mesh.identifier) != m_registeredGeometry.end ())
	{
		Logger::log (LogLevel::Warning, "Trying to re-register geometry.");
		return m_registeredGeometry[mesh.identifier];
	}

	GeometryID const id = m_nextGeometryID++;

	m_geometryIndexBuffer.Insert (id, mesh.vertices.size (), mesh.indices.size ());
	m_geometryToSend.push_back ({id , Geometry {mesh.vertices, mesh.indices}});

	m_registeredGeometry.emplace (mesh.identifier, id);

	size_t indexCount = mesh.indices.size ();

	size_t indexOffset  = m_geometryIndexBuffer.GetIndexOffset  (id);
	size_t vertexOffset = m_geometryIndexBuffer.GetVertexOffset (id);

	m_drawCommandBuffer.RegisterCommand (id, indexCount, vertexOffset, indexOffset);

	return id;
}

InstanceID Renderer::RegisterInstance (GeometryID geometry, Transform const transform, Material const material, AssetManager & assetManager)
{
	if (m_geometryIndexBuffer.find (geometry) == m_geometryIndexBuffer.end ())
	{
		Logger::log (LogLevel::Error, "Geometry does not exists.");
		return InvalidInstance;
	}

	InstanceID const instance = m_nextInstanceID++;

	m_registerdInstances.emplace (instance);
	size_t offset = m_instanceOffsetBuffer.Insert (geometry);
	m_instanceIndexBuffer.Insert (geometry, offset);

	m_instanceToSend.emplace_back
	(
		std::piecewise_construct,
		std::forward_as_tuple (offset),
		std::forward_as_tuple
		(
			transform.GetModelMatrix (),
			CreateInstanceMaterial (material, assetManager)
		)
	);

	// not strictly necessary
	for (auto const & textureHandle : material.diffuseTextures)
	{
		Texture const & texture = assetManager.GetTexture (textureHandle);
		m_atlas.Feed (textureHandle, texture);
	}

	m_instanceToGeometry[instance] = geometry;

	m_drawCommandBuffer.AddInstance (geometry);

	return geometry;
}

void Renderer::RegisterTexture (ResourceHandle handle, Texture const & texture)
{
	m_atlas.Feed (handle, texture);
}

void Renderer::UpdateInstanceMaterial (InstanceID id, Material const & material, AssetManager & assetManager)
{
	size_t base_offset = m_instanceOffsetBuffer.GetOffset (id) * sizeof (Instance);

	m_materialToUpdate.push_back ({base_offset + sizeof (glm::mat4), CreateInstanceMaterial (material, assetManager)});
}

void Renderer::UpdateInstanceTransform (InstanceID id, Transform const & transform)
{
	size_t base_offset = m_instanceOffsetBuffer.GetOffset (id) * sizeof (Instance);

	m_transformToUpdate.push_back ({base_offset, transform.GetModelMatrix ()});
}

void Renderer::UnregisterInstance (InstanceID id)
{
	if (m_registerdInstances.find (id) == m_registerdInstances.end ())
	{
		Logger::log (LogLevel::Warning, "Can't remove Instance, InstanceID was not registered");
		return;
	}

	size_t offset = m_instanceOffsetBuffer.GetOffset (id);
	GeometryID geometry = m_instanceToGeometry[id];

	m_registerdInstances.erase (id);
	m_instanceToGeometry.erase (id);
	m_instanceOffsetBuffer.Remove (id);
	m_instanceIndexBuffer.Remove (geometry, offset);

	m_drawCommandBuffer.RemoveInstance (geometry);
}

void Renderer::Render (Shader const & shader, Camera const & camera)
{
	SyncGPU ();

	//TODO: restore frame buffers functionnalities
	//glBindFramebuffer (GL_FRAMEBUFFER, m_sceneFrameBuffer.Get ());
	glEnable (GL_DEPTH_TEST);
	glClear  (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_vao.Bind  ();
	m_vbo.Bind  ();
	m_ebo.Bind  ();
	m_dibo.Bind ();

	m_ssboInstanceIndex.Bind ();
	m_ssboInstanceData .Bind ();
	m_ssboTextureOffset.Bind ();

	glActiveTexture(GL_TEXTURE0);

	// TODO-fix: add "ShaderInfo" field to change name (not urgent)
	// TODO-fix: add "ShaderParams" field to send custom uniforms (kinda not urgent but probably won't take too long)
	shader.use ();
	shader.upload1i ("uAtlas", 0);
	shader.uploadMatrix4 ("uProjection", m_projection);
	shader.uploadMatrix4 ("uView", camera.getView ());
	
	glBindTexture (GL_TEXTURE_2D_ARRAY, m_atlas.Get ());

	glMultiDrawElementsIndirect (GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, m_drawCommandBuffer.GetCommandCount (), 0);

	shader.cancel ();
	m_vao.Unbind ();
}

Renderer::InstanceMaterial Renderer::CreateInstanceMaterial (Material const & material, AssetManager & assetManager)
{
	// create an index pointing at the begining of a range of textures
	uint32_t diffuseTextureIndex = m_textureIndexBuffer.GetIndex (material.diffuseTextures);
	uint32_t diffuseTextureCount = material.diffuseTextures.size ();

	Hash texturesHash = m_textureIndexBuffer.GetHash (material.diffuseTextures);

	InstanceMaterial value;
	value.color = material.color;
	value.diffuseTextureIndex = diffuseTextureIndex;
	value.diffuseTextureCount = diffuseTextureCount;

	// maybe cache it
	std::vector<TextureOffset> diffuseOffsets;
	for (auto const & textureHandle : material.diffuseTextures)
	{
		Texture const & texture = assetManager.GetTexture (textureHandle);
		m_atlas.Feed (textureHandle, texture);
		diffuseOffsets.emplace_back (textureHandle, m_atlas);
	}

	m_texturesOffsetToSend.emplace
	(
		std::piecewise_construct,
		std::forward_as_tuple (texturesHash),
		std::forward_as_tuple (diffuseTextureIndex, diffuseOffsets)
	);

	return value;
}

void Renderer::SyncGPU ()
{
	m_atlas.Generate ();

	for (auto const & [handle, geometry] : m_geometryToSend)
	{
		size_t vertexOffset = m_geometryIndexBuffer.GetVertexOffset (handle);
		size_t indexOffset  = m_geometryIndexBuffer.GetIndexOffset  (handle);

		Logger::log (LogLevel::Info, "(VBO) Sending Geometry Data (size={}) to gpu at offset={}.", geometry.vertices.size (), vertexOffset);

		m_vbo.Push (geometry.vertices, vertexOffset);
		m_ebo.Push (geometry.indices , indexOffset);
	}

	for (auto const & [index, instance] : m_instanceToSend)
	{
		printMat4 (instance.model);
		m_ssboInstanceData.Push (instance, index);
	}

	for (auto const & [hash, collection] : m_texturesOffsetToSend)
	{
		size_t offset = 0;
		for (auto const & textureOffset : collection.textures)
		{
			m_ssboTextureOffset.Push (textureOffset, collection.index + offset);
			++offset;
		}
	}

	for (auto const & [index, offset] : m_instanceIndexBuffer.Pull ())
	{

		Logger::log (LogLevel::Info, "Trying to push instance offset={} at index={}", offset, index);
		m_ssboInstanceIndex.Push (offset, index);
	}

	for (auto const & [index, material] : m_materialToUpdate)
	{
		Logger::log (LogLevel::Info, "(Material Update) (disabled)");
		//m_ssboInstanceData.Push (material, index);
	}
	
	for (auto const & [index, transform] : m_transformToUpdate)
	{
		Logger::log (LogLevel::Info, "(Transform Update) (disabled)");
		//m_ssboInstanceData.Push (transform, index);
	}

	m_dibo.ReplaceAll (m_drawCommandBuffer.Pull ());

	m_geometryToSend      .clear ();
	m_instanceToSend      .clear ();
	m_texturesOffsetToSend.clear ();
	m_materialToUpdate    .clear ();
	m_transformToUpdate   .clear ();
}
