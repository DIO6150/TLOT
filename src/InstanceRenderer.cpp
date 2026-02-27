#include <engine/data/Vertex.hpp>
#include <engine/data/Mesh.hpp>

#include <engine/opengl/DrawCommand.hpp>

#include <engine/utils/Utils.hpp>

#include <engine/modules/InstanceRenderer.hpp>

#include <glad/glad.h>

#include <iostream>
#include <queue>

using namespace Engine::Core;
using namespace Engine::Data;
using namespace Engine::Module;
using namespace Engine::Internal;

InstanceRenderer::InstanceRenderer (int32_t width, int32_t height):
	mSceneFrameBuffer   {width, height},
	mMaxColorAttachment {1}
	{

	// a bit silly to do that
	AssetManager * am = AssetManager::GetInstance ();
	mFinalBlit = am->LoadShader ("blit_quad", "data/assets/shaders/blit_quad.vertex", "data/assets/shaders/blit_quad.fragment"); // TODO: dont
	mPingPong.reserve (2);


	mPingPong.emplace_back (width, height); // post process A
	mPingPong.emplace_back (width, height); // post process B


	float _quad[] = {
		-1.0,  1.0,   0.0, 1.0,
		-1.0, -1.0,   0.0, 0.0,
		 1.0, -1.0,   1.0, 0.0,

		-1.0,  1.0,   0.0, 1.0,
		 1.0, -1.0,   1.0, 0.0,
		 1.0,  1.0,   1.0, 1.0
	};
	glGenVertexArrays (1, &mFBVAO);
	glGenBuffers (1, &mFBVBO);

	glBindVertexArray (mFBVAO);
	glBindBuffer (GL_ARRAY_BUFFER, mFBVBO);
	glBufferData (GL_ARRAY_BUFFER, sizeof (_quad), _quad, GL_STATIC_DRAW);
	glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof (float), (void *) 0);
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof (float), (void *) (2 * sizeof (float)));
	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);
}

InstanceRenderer::~InstanceRenderer () {
	glDeleteVertexArrays (1, &mFBVAO);
	glDeleteBuffers (1, &mFBVBO);
};

#include <ctime>

void InstanceRenderer::Render (Camera * camera) {
	static int c = 0;
	clock_t dt = std::clock ();
	for (auto & command : mCommands) {
		if (mGroups.find (command.shaderID) == mGroups.end ()) {
			mGroups.emplace (command.shaderID, &mAtlas);
		}
		mGroups.at (command.shaderID).Push (command);
	}
	dt = std::clock () - dt;
	if (c%100 == 0) std::cout << "Feeding " << mCommands.size () << " Mesh(es) took: " << dt * 1000 / CLOCKS_PER_SEC << "ms.\n";
	//if (c%10 == 0) std::cout << "Camera=" << *camera << "\n";

	if (mAtlas.ShouldGenerate ()) {
		mAtlas.Generate ();
		printf ("Regenerated Atlas\n");
	}

	glm::mat4 _view;
	glm::mat4 _projection;

	_view = camera->getView ();
	_projection = glm::perspective(glm::radians (45.0f), 1.0f, 0.1f, 100.0f);

	//glBindFramebuffer (GL_FRAMEBUFFER, mSceneFrameBuffer.Get ());
	glEnable (GL_DEPTH_TEST);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	AssetManager * am = AssetManager::GetInstance ();
	for (auto & [_shaderID, _group] : mGroups) {

		const Shader * _shader = am->GetShader (_shaderID);
		_group.SetTextureRegenerate (false);

		_group.UploadDrawCommands ();
		_group.UploadInstances ();

		_group.Bind ();

		_shader->use ();
		_shader->upload1i	("uAtlas", 0);
		_shader->uploadMatrix4	("uProjection", _projection);
		_shader->uploadMatrix4	("uView", _view);

		glActiveTexture (GL_TEXTURE0 + 0);
		glBindTexture   (GL_TEXTURE_2D_ARRAY, mAtlas.Get ());
		glMultiDrawElementsIndirect (GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _group.GetCommandsCount (), 0);
	
		_shader->cancel ();

		_group.Unbind ();

		_group.ClearInstances ();
	}

	Clear ();
	c++;

	//glBindVertexArray (mFBVAO);
	//glDisable (GL_DEPTH_TEST);
	//glActiveTexture (GL_TEXTURE0);
//
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
	//	const Shader & _postProcess = pAssetManager->GetShader (effect.postProcess);
	//	_postProcess.use ();
	//	_postProcess.upload1i ("uFrame", 0);
//
	//	effect.params.Upload (_postProcess);
//
	//	glBindTexture(GL_TEXTURE_2D, current_color);
//
	//	glDrawArrays (GL_TRIANGLES, 0, 6);
//
	//	_postProcess.cancel ();
	//	current_color = mPingPong[idx].GetColorAttachment (0);
	//	idx = 1 - idx;
	//}
	
	//glBindFramebuffer (GL_FRAMEBUFFER, 0);
	//glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//const Shader & _quadBlit = pAssetManager->GetShader (mFinalBlit);
	//_quadBlit.use ();
	//_quadBlit.upload1i ("uFrame", 0);
	//glBindTexture(GL_TEXTURE_2D, current_color);
	//glDrawArrays (GL_TRIANGLES, 0, 6);
	//glBindVertexArray (0);
}

void InstanceRenderer::PushModel (Instance<Model> & model, const HandleID & shader) {
	if (mInstances.find (model.GetID ()) != mInstances.end ()) {
		// instance already exists, we don't add it back
		return;
	}

	mInstances.insert (model.GetID ());
	for (auto & mesh : model.meshes) {
		//std::cout << "Mesh with verticesID=" << mesh.vertices << " added\n";
		RenderCommand command;
		command.shaderID   = shader;
		command.verticesID = mesh.vertices;
		command.indicesID  = mesh.indices;
		command.material   = mesh.material;
		command.transform  = mesh.transform * model.transform; // maybe not in this order, idk i'm dumb
		command.visible    = mesh.visible; // obviously don't do that

		mCommands.push_back (command);
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
	mInstances.clear ();
	mCommands.clear ();
}



RenderingGroup::RenderingGroup (TextureAtlas * atlas):
	mCurrentVertexCount   {0},
	mCurrentIndexCount    {0},
	mDirtyTextures        {true},
	pAtlas                {atlas}
	{

	glGenVertexArrays(1, &vao);

	glGenBuffers (1, &vbo);
	glGenBuffers (1, &ibo);
	glGenBuffers (1, &dibo);

	glGenBuffers (1, &ssbo);

	Bind ();
	
	std::cout << "Before Allocating Buffers\n";
	// Completly arbitrary values, must allow resizing
	// TODO-asap: allow buffer resizing
	glNamedBufferData (vbo,   10000000 * sizeof (Vertex),      NULL, GL_DYNAMIC_DRAW);
	glNamedBufferData (ibo,   10000000 * sizeof (uint32_t),    NULL, GL_DYNAMIC_DRAW);
	glNamedBufferData (dibo,  100000   * sizeof (DrawCommand), NULL, GL_DYNAMIC_DRAW);
	std::cout << "After Allocating Buffers\n";
	
	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glNamedBufferData (ssbo,  100000   * sizeof (InstanceRenderData), NULL, GL_DYNAMIC_DRAW);

	GenerateAttributes ();
	Unbind ();
}

void RenderingGroup::Push (const RenderCommand & command) {
	// mesh instance count
	if (mKnownGeometry.find (command.verticesID) == mKnownGeometry.end ()) {
		AssetManager * am = AssetManager::GetInstance ();

		// its the first time encoutering this verticesID
		// create a new DrawCommand and allocate memory on the gpu
		// assetManager needs to be a singleton to be allowed access in here

		std::cout << "DrawCommand Generation: VerticesID=" << command.verticesID << "\n";

		const std::vector<Vertex>   _vertices = am->GetVertices (command.verticesID);
		const std::vector<uint32_t> _indices  = am->GetIndices  (command.indicesID);

		//printVector (_vertices, "Vertices");
		//printVector (_indices, "Indices");

		DrawCommand _command;
		_command.index_count    = _indices.size ();
		_command.instance_count = 0;
		_command.base_index     = mCurrentIndexCount;
		_command.base_vertex    = mCurrentVertexCount;
		_command.base_instance  = 0;

		mGeometryToCommands.emplace (command.verticesID, mDrawCommands.size ());
		mDrawCommands.emplace_back (_command);

		// TODO: reallocate buffers if necessary

		glNamedBufferSubData (
			vbo, 
			mCurrentVertexCount * sizeof (Vertex),
			_vertices.size ()   * sizeof (Vertex),
			(void *) (_vertices.data ())
		);
		
		glNamedBufferSubData (
			ibo, 
			mCurrentIndexCount * sizeof (uint32_t),
			_indices.size ()   * sizeof (uint32_t),
			(void *) (_indices.data ())
		);

		mCurrentVertexCount += _vertices.size ();
		mCurrentIndexCount  += _indices .size ();

		mInstances.emplace (command.verticesID, std::vector<InstanceRenderData> {});
		mKnownGeometry.emplace (command.verticesID);
	}

	size_t _commandIndex = mGeometryToCommands.at (command.verticesID);

	// needs to increase number of instances
	DrawCommand & _command = mDrawCommands.at (_commandIndex);
	++_command.instance_count;
	//printf ("command.instance_count=%u\n", _command.instance_count);

	// and offset base_instance of subsequent draw commands
	for (size_t idx = _commandIndex + 1; idx < mDrawCommands.size (); ++idx) {
		mDrawCommands[idx].base_instance = mDrawCommands[idx-1].base_instance + mDrawCommands[idx-1].instance_count;
	}


	// mesh instance data
	// aka transform, textures, material and whatnot
	mDirtyTextures = pAtlas->Feed (command.material.GetTextures ()) || mDirtyTextures;
	mInstances.at (command.verticesID).push_back (InstanceRenderData {
		command.transform,
		CoordsRatio (command.material),
		glm::vec4 {command.material.GetColor (), 1.0}});

	// TODO-fix: expand buffer if not large enough when adding renderables
}

void RenderingGroup::UploadDrawCommands () {
	glNamedBufferSubData (
		dibo,
		0,
		mDrawCommands.size () * sizeof (DrawCommand),
		(void *) (mDrawCommands.data ())
	);
}


void RenderingGroup::UploadInstances () {
	std::vector<InstanceRenderData> _instances;
	
	for (auto & [_, _instanceVec] : mInstances) {
		for (auto & _instanceData : _instanceVec) {
			_instances.push_back (_instanceData);
		}
	}

	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glNamedBufferSubData (
		ssbo,
		0,
		sizeof (InstanceRenderData) * _instances.size (),
		(void*) (_instances.data ())
	);
}

void RenderingGroup::ClearInstances () {
	for (auto & [_, _instanceVec] : mInstances) {
		_instanceVec.clear ();
	}

	for (auto & command : mDrawCommands) {
		command.base_instance = 0;
		command.instance_count = 0;
	}
}

void RenderingGroup::GenerateAttributes () {
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

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
}

glm::vec4 RenderingGroup::CoordsRatio (const Instance<Material> & material) const {
	if (material.GetTextures ().size () > 0) {
		const TextureQuad * _quad =  pAtlas->Quad (material.GetTextures ()[0]);
		if (_quad) {
			float x = _quad->x / pAtlas->Width  ();
			float w = _quad->w / pAtlas->Width  ();

			float y = _quad->y / pAtlas->Height ();
			float h = _quad->h / pAtlas->Height ();
			
			return glm::vec4 {x, y, w, h};
		}
	}

	return glm::vec4 (0.0, 0.0, -1.0, -1.0);

}

void RenderingGroup::Bind () {	
	glBindVertexArray (vao);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindBuffer (GL_DRAW_INDIRECT_BUFFER, dibo);
	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, ssbo);
}

void RenderingGroup::Unbind () {
	glBindVertexArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer (GL_DRAW_INDIRECT_BUFFER, 0);
	glBindBuffer (GL_SHADER_STORAGE_BUFFER, 0);
}

uint32_t RenderingGroup::GetCommandsCount () {
	return mDrawCommands.size ();
}

bool RenderingGroup::ShouldRegenerateTexture () {
	return mDirtyTextures;
}

void RenderingGroup::SetTextureRegenerate (bool flag) {
	mDirtyTextures = flag;
}