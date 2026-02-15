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

RenderingGroup::RenderingGroup (ShaderID shader, const SceneGraph * graph, const Core::AssetManager * manager, const TextureAtlas * atlas) :
	max_vertex_count{2272727U},
	max_index_count {150000U},
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
		Geometry & _geometry = pManager->GetGeometry (_geometryID);
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
}

InstanceRenderer::~InstanceRenderer () {

};

void InstanceRenderer::Init () {
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
}
#include <iostream>
void InstanceRenderer::Render (Camera * camera) {
	glm::mat4 _view;
	glm::mat4 _projection;

	_view = camera->getView ();
	_projection = glm::perspective(glm::radians (45.0f), 1.0f, 0.1f, 100.0f);
	
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
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _group.commands.size (), 0);
	
		_shader->cancel ();

		_group.Unbind ();
	}
}