#include <engine/data/Vertex.hpp>
#include <engine/data/Instance.hpp>
#include <engine/data/Geometry.hpp>
#include <engine/data/Mesh.hpp>
#include <engine/utils/Utils.hpp>

#include <engine/core/Scene.hpp>
#include <engine/core/SceneGraph.hpp>

#include <engine/modules/InstanceRenderer.hpp>

#include <glad/glad.h>

using namespace Engine::Core;
using namespace Engine::Data;
using namespace Engine::Module;
using namespace Engine::Internal;

RenderingGroup::RenderingGroup (ShaderID shader, const SceneGraph * graph, const Core::AssetManager * manager, const TextureAtlas * atlas) :
	max_vertex_count{2 * 2272727U},
	max_index_count {2 * 150000U},
	shader		{shader},
	dirty		{true},
	pScene		{graph},
	pManager	{manager},
	pAtlas		{atlas}
	{

	glGenVertexArrays(1, &vao);

	glGenBuffers (1, &vbo);
	glGenBuffers (1, &ibo);
	glGenBuffers (1, &dibo);

	glGenBuffers (1, &ssbo);
}

void RenderingGroup::Init () {
	size_t _geometryCount	= pScene->GetGeometryCount ();
	size_t _meshCount	= pScene->GetTotalMeshCount ();

	GenerateAttributes ();

	Bind ();

	//TODO-warn: max_vertex_count and max_index_count shall be fetched from scene graph
	glNamedBufferData (vbo,   max_vertex_count   * sizeof (Vertex),      NULL, GL_DYNAMIC_DRAW);
	glNamedBufferData (ibo,   max_index_count    * sizeof (uint32_t),    NULL, GL_DYNAMIC_DRAW);
	glNamedBufferData (dibo,  _geometryCount     * sizeof (DrawCommand), NULL, GL_DYNAMIC_DRAW);

	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glNamedBufferData (ssbo,  _meshCount         * sizeof (Instance),    NULL, GL_DYNAMIC_DRAW);

	Unbind ();
}

void RenderingGroup::GenerateDrawCommands () {
	commands.clear ();
	geometries.clear ();

	uint32_t _vertexCount  = 0;
	uint32_t _indexCount   = 0;
	uint32_t _baseInstance = 0;

	for (const auto &[_geometryID, _] : pScene->GetMeshes ()) {
		const Geometry & _geometry = pManager->GetGeometry (_geometryID);
		uint32_t _instanceCount = pScene->GetInstanceCount (_geometryID);

		commands.emplace_back (
			_geometry.indices.size (),
			_instanceCount,
			_indexCount,
			_vertexCount,
			_baseInstance
		);

		geometries.emplace_back (_geometryID);

		_vertexCount  += _geometry.vertices.size ();
		_indexCount   += _geometry.indices .size ();
		_baseInstance += _instanceCount;
	}
}

void RenderingGroup::UploadBuffers () {
	uint32_t _vertexCount = 0;
	uint32_t _indexCount = 0;

	for (size_t idx = 0; idx < geometries.size (); ++idx) {
		GeometryID _geometryID	= geometries[idx];
		Geometry & _geometry	= pManager->GetGeometry (_geometryID);
		
		glNamedBufferSubData (
			vbo, 
			_vertexCount * sizeof (Vertex),
			_geometry.vertices.size () * sizeof (Vertex),
			(void *) (_geometry.vertices.data ())
		);
		
		glNamedBufferSubData (
			ibo, 
			_indexCount * sizeof (uint32_t),
			_geometry.indices.size () * sizeof (uint32_t),
			(void *) (_geometry.indices.data ())
		);

		_vertexCount += _geometry.vertices.size ();
		_indexCount  += _geometry.indices .size ();
	}

	glNamedBufferSubData (
		dibo,
		0,
		commands.size () * sizeof (DrawCommand),
		(void *) (commands.data ())
	);
}
#include <iostream>
void RenderingGroup::UploadInstance () {
	std::vector<Instance> _instances;
	
	for (size_t idx = 0; idx < geometries.size (); ++idx) {
		GeometryID _geometryID = geometries[idx];
		
		for (const auto & mesh : pScene->GetMeshes ().at (_geometryID)) {
			
			const Material & _material = pManager->GetMaterial (mesh->material);
			
			// TODO-fix: get parent model matrix to get child transform
			// TODO-change: create maybe a buffer of materials ? instead of doing it per instance ? but doing it per instance does have benefit
			_instances.push_back (Instance {
				mesh->position,
				mesh->rotation,
				mesh->scale,
				CoordsRatio (mesh),
				_material.color
			});
		}
	}

	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glNamedBufferSubData (
		ssbo,
		0,
		sizeof (Instance) * _instances.size (),
		(void*) (_instances.data ())
	);
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

glm::vec4 RenderingGroup::CoordsRatio (const Mesh * mesh) const {
	Material _material = pManager->GetMaterial (mesh->material);

	// TODO-fix: generate multiple coords for multiple textures maybe ?
	if (_material.diffuse_textures.size () > 0) {
		const TextureQuad * _quad = pAtlas->Quad (_material.diffuse_textures[0]);
		if (_quad) {
			float x = _quad->x / pAtlas->Width  ();
			float w = _quad->w / pAtlas->Width  ();
	
			float y = _quad->y / pAtlas->Height ();
			float h = _quad->h / pAtlas->Height ();
			
			return (glm::vec4 {x, y, w, h});
		}
	}

	return (glm::vec4 (0.0, 0.0, -1.0, -1.0));

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

InstanceRenderer::InstanceRenderer (Scene * scene):
	mAtlas {&scene->asset_manager},
	IRenderer {scene}
	{
	mFinalBlit = pAssetManager->LoadShader ("blit_quad", "data/assets/shaders/blit_quad.vertex", "data/assets/shaders/blit_quad.fragment");
	mPingPong.reserve (2);
}

InstanceRenderer::~InstanceRenderer () {
	glDeleteVertexArrays (1, &mFBVAO);
	glDeleteBuffers (1, &mFBVBO);
};

void InstanceRenderer::Init (int width, int height) {
	// TODO-fix: generate groups here
	pGraph->Traverse ([this] (Vector<UPtr<Mesh>> & meshes) {
		for (const auto & _mesh : meshes) {
			Material _material = this->pAssetManager->GetMaterial (_mesh->material);
			this->mAtlas.Feed (_material.diffuse_textures);
		}
	});
	mAtlas.Generate ();


	mGroups.clear ();

	mGroups.emplace_back (DefaultShader (), pGraph, pAssetManager, &mAtlas);
	mGroups.back ().Init (); // TODO-fix: assign each mesh to a group

	
	mSceneFrameBuffer.Init (width, height); // scene

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

void InstanceRenderer::Render (Camera * camera) {
	glm::mat4 _view;
	glm::mat4 _projection;

	_view = camera->getView ();
	_projection = glm::perspective(glm::radians (45.0f), 1.0f, 0.1f, 100.0f);

	glBindFramebuffer (GL_FRAMEBUFFER, mSceneFrameBuffer.Get ());
	glEnable (GL_DEPTH_TEST);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for (auto & _group : mGroups) {
		Shader * _shader = &pAssetManager->GetShader (_group.shader);
		// TODO-fix: make it less weird
		if (_group.dirty) {
			_group.GenerateDrawCommands ();
			_group.UploadInstance ();
			_group.UploadBuffers ();
			_group.dirty = false;
		}
		_group.Bind ();

		_shader->use ();
		_shader->upload1i	("uAtlas", 0);
		_shader->uploadMatrix4	("uProjection", _projection);
		_shader->uploadMatrix4	("uView", _view);

		glActiveTexture (GL_TEXTURE0 + 0);
		glBindTexture   (GL_TEXTURE_2D_ARRAY, mAtlas.Get ());
		glMultiDrawElementsIndirect (GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _group.commands.size (), 0);
	
		_shader->cancel ();

		_group.Unbind ();
	}

	glBindVertexArray (mFBVAO);
	glDisable (GL_DEPTH_TEST);
	glActiveTexture (GL_TEXTURE0);

	unsigned int current_color = mSceneFrameBuffer.GetColorAttachement ();
	
	int idx = 0;
	for (auto & [name, effect] : mEffects) {
		if (!effect.active) continue;
		unsigned int _framebuffer = mPingPong[idx].Get ();
		//printf ("framebuffer = %u\n", _framebuffer);
		glBindFramebuffer (GL_FRAMEBUFFER, _framebuffer);
		
		//glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		const Shader & _postProcess = pAssetManager->GetShader (effect.postProcess);
		_postProcess.use ();
		_postProcess.upload1i ("uFrame", 0);
//
		glBindTexture(GL_TEXTURE_2D, current_color);
//
		glDrawArrays (GL_TRIANGLES, 0, 6);
//
		_postProcess.cancel ();
		current_color = mPingPong[idx].GetColorAttachement ();
		idx = 1 - idx;
	}
	
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	const Shader & _quadBlit = pAssetManager->GetShader (mFinalBlit);
	_quadBlit.use ();
	_quadBlit.upload1i ("uFrame", 0);
	glBindTexture(GL_TEXTURE_2D, current_color);
	glDrawArrays (GL_TRIANGLES, 0, 6);
	glBindVertexArray (0);

}

void InstanceRenderer::AddPostProcessingEffect (std::string name, ShaderID shader) {
	mEffects.emplace_back (name, shader);
}

void InstanceRenderer::DisablePostProcessingEffect (std::string name) {

}

void InstanceRenderer::RemovePostProcessingEffect (std::string name) {

}

PostProcessingEffect::PostProcessingEffect (ShaderID shader):
	postProcess {shader},
	active {true}
	{
	
}

FrameBuffer::FrameBuffer () :
	mFrameBuffer {0},
	mTextureColorBuffer {0},
	mRenderBuffer {0}
	{

}

FrameBuffer::FrameBuffer (int width, int height) {
	Init (width, height);
}

FrameBuffer::~FrameBuffer () {
	if (mFrameBuffer) {
		printf ("framebuffer deleted: %u\n", mFrameBuffer);
		glDeleteFramebuffers	(1, &mFrameBuffer);
		glDeleteRenderbuffers	(1, &mRenderBuffer);
		glDeleteTextures	(1, &mTextureColorBuffer);
	}
}

void FrameBuffer::Init (int width, int height) {
	glGenFramebuffers (1, &mFrameBuffer);
	glBindFramebuffer (GL_FRAMEBUFFER, mFrameBuffer);

	glGenTextures (1, &mTextureColorBuffer);
	glBindTexture (GL_TEXTURE_2D, mTextureColorBuffer);
	glTexImage2D  (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture (GL_TEXTURE_2D, 0);

	glGenRenderbuffers (1, &mRenderBuffer);
	glBindRenderbuffer (GL_RENDERBUFFER, mRenderBuffer);
	glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer (GL_RENDERBUFFER, 0);

	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureColorBuffer, 0);
	glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderBuffer);

	if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf ("SceneFrameBuffer is not complete :(\n");
	}
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
}

unsigned int FrameBuffer::Get () const {
	return (mFrameBuffer);
}

unsigned int FrameBuffer::GetColorAttachement () const {
	return (mTextureColorBuffer);
}