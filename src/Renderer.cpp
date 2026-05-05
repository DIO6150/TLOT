#include <modules/Renderer.hpp>

#include <core/Logger.hpp>

#include <core/Utils.hpp>

#include <chrono>

using namespace std::chrono;

using namespace TLOT;

Renderer::Renderer (size_t windowWidth, size_t windowHeight, ProjectionMode mode):
	m_mode {mode}
{
	if (mode == ProjectionMode::Perspective)
	{
		m_projection = glm::perspective (glm::radians (45.0f), (float)windowWidth / windowHeight, 0.1f, 100.0f);
	}
	else if (mode == ProjectionMode::Orthogonal)
	{
		m_projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f);
	}

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
		return m_registeredGeometry[mesh.identifier];
	}

	GeometryID const geometry = m_nextGeometryID++;

	m_geometryIndexBuffer.Insert (geometry, mesh.vertices.size (), mesh.indices.size ());
	m_geometryToSend.push_back ({geometry , Geometry {mesh.vertices, mesh.indices}});

	m_registeredGeometry.emplace (mesh.identifier, geometry);

	size_t indexCount = mesh.indices.size ();

	size_t indexOffset  = m_geometryIndexBuffer.GetIndexOffset  (geometry);
	size_t vertexOffset = m_geometryIndexBuffer.GetVertexOffset (geometry);

	m_drawCommandBuffer.RegisterCommand (geometry, indexCount, vertexOffset, indexOffset);

	m_shouldSkipSync = false;

	Logger::log (LogLevel::Info, "Created geometry with id= {}", geometry);

	return geometry;
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
	size_t offset = m_instanceOffsetBuffer.Insert (instance);
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

	m_instanceToGeometry[instance] = geometry;

	m_drawCommandBuffer.AddInstance (geometry);

	m_shouldSkipSync = false;

	return instance;
}

void Renderer::RegisterTexture (ResourceHandle handle, Texture const & texture)
{
	if (handle == 0) Logger::log (LogLevel::Warning, "defaultTexture from RegisterTexture");
	m_atlas.Feed (handle, texture);
}

void Renderer::UpdateInstanceMaterial (InstanceID id, Material const & material, AssetManager & assetManager)
{
	size_t base_offset = m_instanceOffsetBuffer.GetOffset (id) * sizeof (Instance);

	m_materialToUpdate.push_back ({base_offset, CreateInstanceMaterial (material, assetManager)});

	m_shouldSkipSync = false;
}

void Renderer::UpdateInstanceTransform (InstanceID id, Transform transform)
{
	size_t base_offset = m_instanceOffsetBuffer.GetOffset (id);

	m_transformToUpdate.push_back ({base_offset, transform.GetModelMatrix ()});

	m_shouldSkipSync = false;
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

	//TODO: maybe remove TextureOffsets ? idk

	m_shouldSkipSync = false;
}

void Renderer::Render (Shader const & shader, Camera const & camera)
{
	SyncGPU ();

	//TODO: restore frame buffers functionnalities
	//glBindFramebuffer (GL_FRAMEBUFFER, m_sceneFrameBuffer.Get ());
	glEnable (GL_DEPTH_TEST);

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
	if (m_mode == ProjectionMode::Perspective) shader.uploadMatrix4 ("uView", camera.getView ());
	
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

	if (!texturesHash)
	{
		Logger::log (LogLevel::Error, "Hash is null, texture offset is incorrect");
	}

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
	if (m_shouldSkipSync) return;

	m_atlas.Generate ();

	// one way to optimize this is to merge all consecutive data beforehand and send that instead of every little things

	for (auto const & [handle, geometry] : m_geometryToSend)
	{
		size_t vertexOffset = m_geometryIndexBuffer.GetVertexOffset (handle);
		size_t indexOffset  = m_geometryIndexBuffer.GetIndexOffset  (handle);

		//Logger::log (LogLevel::Info, "(VBO) Sending Geometry Data (size={}) to gpu at offset={}.", geometry.vertices.size (), vertexOffset);

		m_vbo.Push (geometry.vertices, vertexOffset);
		m_ebo.Push (geometry.indices , indexOffset);
	}

	for (auto const & [index, instance] : m_instanceToSend)
	{
		m_ssboInstanceData.Push (instance, index);
	}

	for (auto const & [hash, collection] : m_texturesOffsetToSend)
	{
		size_t offset = 0;
		for (auto const & textureOffset : collection.textures)
		{
			m_ssboTextureOffset.Push (textureOffset, collection.index + offset++);
		}
	}

	for (auto const & [index, offset] : m_instanceIndexBuffer.Pull ())
	{

		//Logger::log (LogLevel::Info, "Trying to push instance offset={} at index={}", offset, index);
		m_ssboInstanceIndex.Push (offset, index);
	}

	for (auto const & [index, material] : m_materialToUpdate)
	{
		//Logger::log (LogLevel::Info, "(Material Update) (disabled)");
		m_ssboInstanceData.PushPart (material, index, sizeof (glm::mat4));
	}
	
	for (auto const & [index, transform] : m_transformToUpdate)
	{
		m_ssboInstanceData.PushPart (transform, index, 0);
	}

	m_dibo.ReplaceAll (m_drawCommandBuffer.Pull ());

	m_geometryToSend      .clear ();
	m_instanceToSend      .clear ();
	m_texturesOffsetToSend.clear ();
	m_materialToUpdate    .clear ();
	m_transformToUpdate   .clear ();

	m_shouldSkipSync = true;
}

Renderer::~Renderer ()
{
	
}
