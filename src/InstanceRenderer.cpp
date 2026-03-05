#include <engine/data/Vertex.hpp>
#include <engine/data/Mesh.hpp>

#include <engine/opengl/DrawCommand.hpp>

#include <engine/utils/Utils.hpp>

#include <engine/modules/InstanceRenderer.hpp>

#include <glad/glad.h>

#include <ctime>
#include <iostream>
#include <queue>

using namespace Engine::Core;
using namespace Engine::Data;
using namespace Engine::Module;
using namespace Engine::Internal;

static glm::vec4 getTextureCoords (const Instance<Material> & material, const TextureAtlas * atlas) {
	if (material.GetTextures ().size () > 0) {
		const TextureQuad * _quad =  atlas->Quad (material.GetTextures ()[0]);
		if (_quad) {
			float x = _quad->x / atlas->Width  ();
			float w = _quad->w / atlas->Width  ();

			float y = _quad->y / atlas->Height ();
			float h = _quad->h / atlas->Height ();
			
			return glm::vec4 {x, y, w, h};
		}
	}

	return {0.0, 0.0, 0.0, 0.0};
}

InstanceRenderer::InstanceRenderer (int32_t width, int32_t height):
	mSceneFrameBuffer   {width, height},
	mMaxColorAttachment {1}
	{

	glGenVertexArrays(1, &vao);

	glGenBuffers (1, &vbo);
	glGenBuffers (1, &ebo);

	glBindVertexArray (vao);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo);

	// POSITIONS
	glVertexAttribPointer (
		0, 
		3,
		GL_FLOAT,
		GL_FALSE,
		11 * sizeof (float),
		(void *) 0
	);

	// NORMALS
	glVertexAttribPointer (
		1, 
		3,
		GL_FLOAT,
		GL_FALSE,
		11 * sizeof (float),
		(void *) (3 * sizeof (float))
	);

	// TANGEANTS
	glVertexAttribPointer (
		2, 
		3,
		GL_FLOAT,
		GL_FALSE,
		11 * sizeof (float),
		(void *) (6 * sizeof (float))
	);

	// UV COORDINATES
	glVertexAttribPointer (
		3, 
		2,
		GL_FLOAT,
		GL_FALSE,
		11 * sizeof (float),
		(void *) (9 * sizeof (float))
	);

	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);
	glEnableVertexAttribArray (2);
	glEnableVertexAttribArray (3);

	// Completly arbitrary values, must allow resizing
	// TODO-asap: allow buffer resizing
	glNamedBufferData (vbo,   100000 * sizeof (Vertex),      NULL, GL_DYNAMIC_DRAW);
	glNamedBufferData (ebo,   500000 * sizeof (uint32_t),    NULL, GL_DYNAMIC_DRAW);

	glBindVertexArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);

	// a bit silly to do that
	AssetManager * am = AssetManager::GetInstance ();
	mFinalBlit = am->LoadShader ("blit_quad", "data/assets/shaders/blit_quad.vertex", "data/assets/shaders/blit_quad.fragment"); // TODO: dont
	mPingPong.reserve (2);


	mPingPong.emplace_back (width, height); // post process A
	mPingPong.emplace_back (width, height); // post process B


	//float _quad[] = {
	//	-1.0,  1.0,   0.0, 1.0,
	//	-1.0, -1.0,   0.0, 0.0,
	//	 1.0, -1.0,   1.0, 0.0,
//
	//	-1.0,  1.0,   0.0, 1.0,
	//	 1.0, -1.0,   1.0, 0.0,
	//	 1.0,  1.0,   1.0, 1.0
	//};
	//glGenVertexArrays (1, &mFBVAO);
	//glGenBuffers (1, &mFBVBO);
//
	//glBindVertexArray (mFBVAO);
	//glBindBuffer (GL_ARRAY_BUFFER, mFBVBO);
	//glBufferData (GL_ARRAY_BUFFER, sizeof (_quad), _quad, GL_STATIC_DRAW);
	//glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof (float), (void *) 0);
	//glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof (float), (void *) (2 * sizeof (float)));
	//glEnableVertexAttribArray (0);
	//glEnableVertexAttribArray (1);
	//glBindVertexArray (0);
	//glBindBuffer (GL_ARRAY_BUFFER, 0);

	mDebugMeshCounter = 0;
}

InstanceRenderer::~InstanceRenderer () {
	glDeleteVertexArrays (1, &mFBVAO);
	glDeleteBuffers (1, &mFBVBO);
};

void InstanceRenderer::Render (Camera * camera) {
	static int c = 0;

	clock_t dtFrameBegin = std::clock ();

	AssetManager * am = AssetManager::GetInstance ();

	if (mAtlas.ShouldGenerate ()) {
		mAtlas.Generate ();
		printf ("Regenerated Atlas\n");
	}

	glm::mat4 _view;
	glm::mat4 _projection;

	_view = camera->getView ();
	_projection = glm::perspective(glm::radians (45.0f), 1.0f, 0.1f, 100.0f);

	//glBindFramebuffer (GL_FRAMEBUFFER, mSceneFrameBuffer.Get ());
	//glEnable (GL_DEPTH_TEST);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glBindVertexArray (vao);

	for (auto & [_shaderID, _group] : mGroups) {
		
		_group.UploadDrawCommands ();
		_group.UploadInstances    ();
		
		_group.Bind ();
		
		const Shader * _shader = am->GetShader (_shaderID);
		_shader->use ();
		_shader->upload1i	("uAtlas", 0);
		_shader->uploadMatrix4	("uProjection", _projection);
		_shader->uploadMatrix4	("uView", _view);

		glBindTexture (GL_TEXTURE_2D_ARRAY, mAtlas.Get ());
		glMultiDrawElementsIndirect (GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, _group.GetCommandsCount (), 0);
	
		_shader->cancel ();

		_group.Unbind ();

		_group.debugUpdate ();
	}

	//glBindVertexArray (mFBVAO);
	//glDisable (GL_DEPTH_TEST);
	//glActiveTexture (GL_TEXTURE0);

	//unsigned int current_color = mSceneFrameBuffer.GetColorAttachment (0);
	//
	//int idx = 0;
	//for (auto & [name, effect] : mEffects) {
	//	if (!effect.active) continue;
	//	unsigned int _framebuffer = mPingPong[idx].Get ();
	//	//printf ("framebuffer = %u\n", _framebuffer);
	//	glBindFramebuffer (GL_FRAMEBUFFER, _framebuffer);
	//	
	//	//glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	const Shader * _postProcess = am->GetShader (effect.postProcess);
	//	glBindTexture(GL_TEXTURE_2D, current_color);
//
	//	_postProcess->use ();
	//	_postProcess->upload1i ("uFrame", 0);
//
	//	effect.params.Upload (*_postProcess);
//
//
	//	glDrawArrays (GL_TRIANGLES, 0, 6);
//
	//	_postProcess->cancel ();
	//	current_color = mPingPong[idx].GetColorAttachment (0);
	//	idx = 1 - idx;
	//}
	//glBindFramebuffer (GL_FRAMEBUFFER, 0);
	//glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glBindTexture(GL_TEXTURE_2D, current_color);
	//const Shader * _quadBlit = am->GetShader (mFinalBlit);
	//_quadBlit->use ();
	//_quadBlit->upload1i ("uFrame", 0);
	//glDrawArrays (GL_TRIANGLES, 0, 6);
	//glBindVertexArray (0);

	dtFrameBegin = std::clock () - dtFrameBegin;
	if (c%100 == 0) std::cout << "Rendering " << mDebugMeshCounter << " Mesh(es) took: " << dtFrameBegin * 1000 / CLOCKS_PER_SEC << "ms.\n";

	Clear ();
	c++;
}

void InstanceRenderer::RegisterGeometry (const Model & model) {
	auto * am = AssetManager::GetInstance ();
	
	for (const auto & _meshID : model.meshes) {
		const auto * _mesh = am->GetMesh (_meshID);
		RegisterGeometry (_mesh->vertices, _mesh->indices);
	}
}

void InstanceRenderer::RegisterGeometry (const HandleID & verticesID, const HandleID & indicesID) {
	if (mGeometryBuffer.find (verticesID) != mGeometryBuffer.end ()) return;
	mGeometryBuffer.insert (verticesID, indicesID);

	uint32_t _vertexOffset = mGeometryBuffer.GetVertexOffset (verticesID);
	uint32_t _indexOffset  = mGeometryBuffer.GetIndexOffset  (indicesID);

	auto _assetManager = AssetManager::GetInstance ();

	auto _vertices = _assetManager->GetVertices (verticesID);
	auto _indices = _assetManager->GetIndices  (indicesID);

	glNamedBufferSubData (vbo, _vertexOffset * sizeof (Vertex)  , _vertices.size () * sizeof (Vertex)  , (void *) _vertices.data ());
	glNamedBufferSubData (ebo, _indexOffset  * sizeof (uint32_t), _indices .size () * sizeof (uint32_t), (void *) _indices .data ());
}

void InstanceRenderer::PushModel (Instance<Model> & model, const HandleID & shaderID) {
	mDebugMeshCounter += model.meshes.size ();

	if (mInstances.find (model.GetID ()) != mInstances.end ()) {
		// instance already exists, we don't add it back this frame
		return;
	}

	mInstances.insert (model.GetID ());
	for (auto & mesh : model.meshes) {
		if (!mesh.visible) {
			// remove if needed in corresponding group
			continue;
		}

		RegisterGeometry (mesh.vertices, mesh.indices);
		mAtlas.Feed (mesh.material.GetTextures ());
		
		RenderCommand command;
		command.instanceID   = mesh.GetID ();
		command.vertexOffset = mGeometryBuffer.GetVertexOffset (mesh.vertices);
		command.indexOffset  = mGeometryBuffer.GetIndexOffset  (mesh.indices);
		command.verticesID   = mesh.vertices;
		command.indicesID    = mesh.indices;
		command.transform    = mesh.GetMatrix () * model.GetMatrix (); // maybe not in this order, idk i'm dumb
		command.textureCoord = getTextureCoords (mesh.material, &mAtlas); 
		command.color        = glm::vec4 (mesh.material.GetColor (), 1.0);
		
		mGroups [shaderID].Push (command);

		mMeshInstancesToShader.emplace (mesh.GetID (), shaderID);
	}
}

void InstanceRenderer::UpdateModelTransform (Instance<Model> & model) {
	if (mInstances.find (model.GetID ()) == mInstances.end ()) {
		// instance does not exist, we don't need to update it
		return;
	}

	// for each mesh we need to find its group:
	for (const auto & _meshInstance : model.meshes) {
		HandleID & _shaderID = mMeshInstancesToShader.at (_meshInstance.GetID ());
		RenderingGroup & _group = mGroups[_shaderID];

		_group.UpdateInstanceTransform (_meshInstance.GetID (), _meshInstance.GetMatrix () * model.GetMatrix ());
	}
}

void InstanceRenderer::AddPostProcessingEffect (std::string name, Core::HandleID shader) {
	mEffects.emplace_back (std::piecewise_construct, std::forward_as_tuple (name), std::forward_as_tuple (shader, 1));
}

void InstanceRenderer::AddPostProcessingEffect (std::string name, HandleID shader, size_t colorAttachmentCount, ShaderInputs && params) {
	mEffects.emplace_back (std::piecewise_construct, std::forward_as_tuple (name), std::forward_as_tuple (shader, colorAttachmentCount, std::move (params)));

	while (colorAttachmentCount > mMaxColorAttachment) {
		for (auto & target: mPingPong) {
			target.AddColorAttachment ();
		}
		++mMaxColorAttachment;
	}
}

void InstanceRenderer::DisablePostProcessingEffect (std::string name) {
	//TODO-implement
}

void InstanceRenderer::RemovePostProcessingEffect (std::string name) {
	//TODO-implement
}

void InstanceRenderer::Clear () {
	mDebugMeshCounter = 0;
}



RenderingGroup::RenderingGroup ():
	mDirtyCommand {0},
	mCounter {0} {

	glGenBuffers (1, &dibo);
	glGenBuffers (1, &ssbo);

	glBindBuffer (GL_DRAW_INDIRECT_BUFFER, dibo);
	glBindBuffer (GL_SHADER_STORAGE_BUFFER, ssbo);
	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	
	// Completly arbitrary values, must allow resizing
	// TODO-asap: allow buffer resizing
	glNamedBufferData (dibo,  100000   * sizeof (DrawCommand), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glNamedBufferData (ssbo,  100000   * sizeof (InstanceRenderData), NULL, GL_DYNAMIC_DRAW);
	
	glBindBuffer (GL_DRAW_INDIRECT_BUFFER, 0);
	glBindBuffer (GL_SHADER_STORAGE_BUFFER, 0);
}

void RenderingGroup::CreateDrawCommand (const HandleID & verticesID, const std::vector<Vertex> & vertices, const std::vector<uint32_t> indices, const uint32_t vertexOffset, const uint32_t indexOffset) {
	std::cout << "DrawCommand Generation: VerticesID=" << verticesID << "\n";

	mDrawCommands.emplace_back (
		indices.size (),
		0,
		indexOffset,
		vertexOffset,
		0
	);

	mCommandsToGeometry	.emplace_back (verticesID);
	mGeometryToCommands	.emplace (verticesID, mDrawCommands.size () - 1);
}

void RenderingGroup::DecreaseDCInstance (const HandleID & verticesID) {
	auto _idx = mGeometryToCommands.at (verticesID);
	--mDrawCommands[_idx].instance_count;
	mDirtyCommand = _idx;
}

void RenderingGroup::Push (const RenderCommand & command) {
	// command stuff
	if (mGeometryToCommands.find (command.verticesID) == mGeometryToCommands.end ()) {
		auto _assetManager = AssetManager::GetInstance ();
		auto _vertices = _assetManager->GetVertices (command.verticesID);
		auto _indices = _assetManager->GetIndices (command.indicesID);
		CreateDrawCommand (command.verticesID, _vertices, _indices, command.vertexOffset, command.indexOffset);

		mNextFreePosition.emplace (command.verticesID, 0);
	}

	if (mInstances.find (command.instanceID) != mInstances.end ()) {
		return;
	}	

	// increment instance count
	auto _idx = mGeometryToCommands.at (command.verticesID);
	++mDrawCommands[_idx].instance_count;
	mDirtyCommand = mDirtyCommand < _idx ? mDirtyCommand : _idx;
	

	// instance stuff
	mInstances.emplace (command.instanceID, InstanceRenderData {
		command.transform,
		command.textureCoord,
		command.color
	});

	mInstancesToGeometry.emplace (command.instanceID, command.verticesID);
	mGeometryToInstances.emplace (command.verticesID, command.instanceID);

	// position assignation

	size_t _position;

	// if no positions were previously freed
	if (mFreePositions.lower_bound (command.verticesID) == mFreePositions.upper_bound (command.verticesID)) {
		_position = mNextFreePosition.at (command.verticesID);
		++mNextFreePosition[command.verticesID];
	}
	else {
		auto it = mFreePositions.lower_bound (command.verticesID);
		_position = it->second;

		mFreePositions.erase (it);
	}

	mPositionToInstance.emplace (_position, command.instanceID);
	mInstanceToPosition.emplace (command.instanceID, _position);
	mDirtyInstances    .emplace (command.instanceID);
}

void RenderingGroup::UploadDrawCommands () {
	if (mDirtyCommand == mDrawCommands.size ()) return;

	clock_t deltaTime = std::clock ();
	for (auto _idx = mDirtyCommand + 1; _idx < mDrawCommands.size (); ++_idx) {
		mDrawCommands[_idx].base_instance = mDrawCommands[_idx - 1].base_instance + mDrawCommands[_idx - 1].instance_count;
	}

	glNamedBufferSubData (
		dibo,
		mDirtyCommand * sizeof (DrawCommand),
		(mDrawCommands.size () - mDirtyCommand) * sizeof (DrawCommand),
		(void *) (mDrawCommands.data () + mDirtyCommand)
	);

	mDirtyCommand = mDrawCommands.size ();

	deltaTime = std::clock () - deltaTime;
	if (mCounter % 100 == 0) std::cout << "Uploaded " << mDirtyCommand << " Draw Commands in " << deltaTime << "ms \n";
}


void RenderingGroup::UploadInstances () {
	if (mDirtyInstances.empty ()) return;

	clock_t deltaTime = std::clock ();
	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	for (auto & _instanceID : mDirtyInstances) {
		const HandleID & _geometryID = mInstancesToGeometry.at (_instanceID);
		size_t _commandIdx = mGeometryToCommands.at (_geometryID);
		size_t _baseIdx = mDrawCommands[_commandIdx].base_instance;
		size_t _instanceIdx = mInstanceToPosition.at (_instanceID);

		//std::cout << "(upload) Mesh " << _instanceID << ", local position = " << _instanceIdx << " and global pos = " << _baseIdx + _instanceIdx << "\n";

		InstanceRenderData & _ird = mInstances.at (_instanceID);
		glNamedBufferSubData (
			ssbo,
			(_baseIdx + _instanceIdx) * sizeof (InstanceRenderData),
			sizeof (InstanceRenderData),
			(void*) (&_ird)
		);
	}

	deltaTime = std::clock () - deltaTime;
	if (mCounter % 100  == 0) std::cout << "Uploaded " << mDirtyInstances.size () << " instance data in " << deltaTime << "ms \n";
	mDirtyInstances.clear ();
}

void RenderingGroup::UpdateInstanceTransform (uint64_t meshID, glm::mat4 transform) {
	const auto & pos = mInstances.find (meshID);

	assert (mInstances.find (meshID) != mInstances.end ());

	InstanceRenderData & ird = pos->second;
	ird.transform = transform;
	mDirtyInstances.insert (meshID);
}

void RenderingGroup::Bind () {	
	glBindBuffer (GL_DRAW_INDIRECT_BUFFER, dibo);
	glBindBuffer (GL_SHADER_STORAGE_BUFFER, ssbo);
	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, ssbo);
}

void RenderingGroup::Unbind () {
	glBindBuffer (GL_DRAW_INDIRECT_BUFFER, 0);
	glBindBuffer (GL_SHADER_STORAGE_BUFFER, 0);
}

uint32_t RenderingGroup::GetCommandsCount () {
	return mDrawCommands.size ();
}

