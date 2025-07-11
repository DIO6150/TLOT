#include "batch.hpp"

#include "glad/glad.h"

#include <details/draw_command.hpp>
#include <details/instance_data.hpp>

#include <layout.hpp>
#include <mesh.hpp>
#include <utils.hpp>
#include <game_object.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

Engine::Batch &Engine::Batch::operator= (const Engine::Batch other) {
	m_instance_counter = other.m_instance_counter;
	m_loaded_objects = other.m_loaded_objects;

	m_should_resend_ssbo = other.m_should_resend_ssbo;
	m_visible_mesh_count = other.m_visible_mesh_count;
	m_vertex_count = other.m_vertex_count;
	m_index_count = other.m_index_count;

	m_vao = other.m_vao;
	m_vbo = other.m_vbo;
	m_ebo = other.m_ebo;
	m_dibo = other.m_dibo;
	m_instance_ssbo = other.m_instance_ssbo;

	m_diffuseMap = other.m_diffuseMap;
	m_ambientMap = other.m_ambientMap;
	m_normalMap = other.m_normalMap;
	m_specularMap = other.m_specularMap;
	m_albedoMap = other.m_albedoMap;

	m_shader = other.m_shader;
	m_layout = other.m_layout;

	m_parent_scene = other.m_parent_scene;

	m_hash = other.m_hash;

	return (*this);
}

Engine::Batch::Batch (const Engine::Batch &other) {
	m_instance_counter = other.m_instance_counter;
	m_loaded_objects = other.m_loaded_objects;
	
	m_should_resend_ssbo = other.m_should_resend_ssbo;
	m_visible_mesh_count = other.m_visible_mesh_count;
	m_vertex_count = other.m_vertex_count;
	m_index_count = other.m_index_count;

	m_vao = other.m_vao;
	m_vbo = other.m_vbo;
	m_ebo = other.m_ebo;
	m_dibo = other.m_dibo;
	m_instance_ssbo = other.m_instance_ssbo;

	m_diffuseMap = other.m_diffuseMap;
	m_ambientMap = other.m_ambientMap;
	m_normalMap = other.m_normalMap;
	m_specularMap = other.m_specularMap;
	m_albedoMap = other.m_albedoMap;

	m_shader = other.m_shader;
	m_layout = other.m_layout;

	m_parent_scene = other.m_parent_scene;

	m_hash = other.m_hash;
}

Engine::Batch::Batch () :
	m_should_resend_ssbo (true),
	m_visible_mesh_count (0),
	m_vertex_count (0),
	m_index_count (0) {
	
}

Engine::Batch::Batch (Shader* shader_base) :
	Batch (nullptr, shader_base) {
	
}

Engine::Batch::Batch (Layout *&&layout, Shader* shader_base) :
	m_should_resend_ssbo (true),
	m_visible_mesh_count (0),
	m_vertex_count (0),
	m_index_count (0),
	m_shader (shader_base) {

	m_layout = std::move (layout);
	layout = nullptr;	

	size_t vbo_size;
	size_t ebo_size;
	size_t dibo_size;

	glGenVertexArrays(1, &m_vao);

	glGenBuffers (1, &m_vbo);
	glGenBuffers (1, &m_ebo);
	glGenBuffers (1, &m_dibo);

	glGenBuffers (1, &m_instance_ssbo);


	vbo_size	= 1;
	ebo_size	= 1;
	dibo_size	= MAX_INSTANCE_COUNT * sizeof (EngineDetail::DrawCommand);

	Bind ();

	glNamedBufferData (m_vbo,   vbo_size,  NULL, GL_DYNAMIC_DRAW);
	glNamedBufferData (m_ebo,   ebo_size,  NULL, GL_DYNAMIC_DRAW);
	glNamedBufferData (m_dibo,  dibo_size, NULL, GL_DYNAMIC_DRAW);

	glBindBuffer (GL_SHADER_STORAGE_BUFFER, m_instance_ssbo);
	glNamedBufferData (m_instance_ssbo, MAX_INSTANCE_COUNT * sizeof (EngineDetail::InstanceData), NULL, GL_DYNAMIC_DRAW);

	m_layout->Apply ();

	Unbind ();
}

Engine::Batch::Batch (Layout *&&layout, std::vector<Mesh *> &meshes, Shader* shader_base) :
	m_should_resend_ssbo (true),
	m_visible_mesh_count (0),
	m_vertex_count (0),
	m_index_count (0),
	m_shader (shader_base) {

	m_layout = std::move (layout);
	layout = nullptr;

	size_t vbo_size;
	size_t ebo_size;
	size_t dibo_size;
	
	std::vector<Vertex>	vertices;
	std::vector<uint32_t> 	indices;


	glGenVertexArrays(1, &m_vao);

	glGenBuffers (1, &m_vbo);
	glGenBuffers (1, &m_ebo);
	glGenBuffers (1, &m_dibo);

	glGenBuffers (1, &m_instance_ssbo);


	for (auto &mesh : meshes) {
		if (m_instance_counter.find (mesh) != m_instance_counter.end ()) continue;

		vertices.insert (vertices.end (), mesh->vertices.begin (), mesh->vertices.end ());
		indices .insert (indices .end (), mesh->indices .begin (), mesh->indices .end ());

		m_instance_counter.insert ({mesh, 0});
	}


	vbo_size	= vertices.size () 	* sizeof (Vertex);
	ebo_size	= indices .size () 	* sizeof (uint32_t);
	dibo_size	= MAX_INSTANCE_COUNT 	* sizeof (EngineDetail::DrawCommand);

	Bind ();

	glNamedBufferData (m_vbo,   vbo_size,  vertices.data (), GL_DYNAMIC_DRAW);
	glNamedBufferData (m_ebo,   ebo_size,  indices .data (), GL_DYNAMIC_DRAW);
	glNamedBufferData (m_dibo,  dibo_size, NULL,             GL_DYNAMIC_DRAW);

	glBindBuffer (GL_SHADER_STORAGE_BUFFER, m_instance_ssbo);
	glNamedBufferData (m_instance_ssbo, MAX_INSTANCE_COUNT * sizeof (EngineDetail::InstanceData), NULL, GL_DYNAMIC_DRAW);

	m_layout->Apply ();

	Unbind ();
}

Engine::Batch::~Batch () {
	// vbo, ebo, dibo and instance_ssbo all are contiguous in memory and therefore has no technical difference when using an array.
	// it's what I thought to be the case but it seems to be more complicated than that
	// we'll juste delete them one by one to be safe

	glDeleteBuffers (1, &m_vbo);
	glDeleteBuffers (1, &m_ebo);
	glDeleteBuffers (1, &m_dibo);
	glDeleteBuffers (1, &m_instance_ssbo);

	delete m_layout;
}

void Engine::Batch::UploadGameObject (GameObject *object) {
	std::unordered_map<Mesh *, uint32_t>::iterator pos;
	if ((pos = m_instance_counter.find (object->m_mesh)) == m_instance_counter.end ()) {
		// Uploading new geometry to the gpu
		Expand (object->m_mesh);

		UploadMesh (object->m_mesh);

		m_instance_counter.insert ({object->m_mesh, 1});
		std::cout << "new geometry uploaded\n";
	}
	else {
		++pos->second;
	}
	
	++m_visible_mesh_count;
	object->m_parent_batch = this;
	m_loaded_objects.push_back (object);
}

void Engine::Batch::Expand (Mesh *target_mesh) {
	void* old_vbo_data = NULL;
	void* old_ebo_data = NULL;

	if (m_vertex_count > 0 && m_index_count > 0) {
		old_vbo_data = glMapNamedBuffer(m_vbo, GL_READ_WRITE);
		old_ebo_data = glMapNamedBuffer(m_ebo, GL_READ_WRITE);

		if (!old_vbo_data || !old_ebo_data) {
			// TODO : error handling + logging	
			exit (-1);
		}

		glUnmapNamedBuffer (m_vbo);
		glUnmapNamedBuffer (m_ebo);
	}
	
	uint32_t new_vertex_count	= m_vertex_count + target_mesh->vertices.size ();
	uint32_t new_index_count	= m_index_count  + target_mesh->indices .size ();

	std::cout << new_vertex_count << "  :   " << new_index_count << "\n";

	glNamedBufferData (m_vbo, (new_vertex_count) * sizeof (Vertex),   NULL, GL_DYNAMIC_DRAW);
	glNamedBufferData (m_ebo, (new_index_count)  * sizeof (uint32_t), NULL, GL_DYNAMIC_DRAW);

	if (m_vertex_count > 0 && m_index_count > 0) {
		glNamedBufferSubData (m_vbo, 0, m_vertex_count * sizeof (Vertex),   old_vbo_data);
		glNamedBufferSubData (m_ebo, 0, m_index_count  * sizeof (uint32_t), old_ebo_data);
	}

	static int called = 0;
	std::cout << ++called << "\n";

}

void Engine::Batch::UploadMesh (Mesh *mesh) {
	glNamedBufferSubData (m_vbo, m_vertex_count * sizeof (Vertex),   mesh->vertices.size () * sizeof (Vertex),   mesh->vertices.data ());
	glNamedBufferSubData (m_ebo, m_index_count  * sizeof (uint32_t), mesh->indices .size () * sizeof (uint32_t), mesh->indices .data ());

	m_vertex_count += mesh->vertices.size ();
	m_index_count  += mesh->indices .size ();
}

void Engine::Batch::Bind () const {
	glBindVertexArray (m_vao);
	glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBindBuffer (GL_DRAW_INDIRECT_BUFFER, m_dibo);
	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, m_instance_ssbo);
}

void Engine::Batch::Unbind () const {
	glBindVertexArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer (GL_DRAW_INDIRECT_BUFFER, 0);
	glBindBuffer (GL_SHADER_STORAGE_BUFFER, 0);
}

uint32_t Engine::Batch::GetDiffuse () const {
	return (m_diffuseMap);
}

uint32_t Engine::Batch::GetAmbient () const {
	return (m_ambientMap);
}

Engine::Shader *Engine::Batch::GetShader () const {
	return (m_shader);
}

void Engine::Batch::UploadDrawCommands () const {
	size_t		index_offset;
	size_t		vertex_offset;
	size_t		instance_offset;
	intptr_t 	draw_commands_offset;

	EngineDetail::DrawCommand command;

	vertex_offset   	= 0;
	index_offset    	= 0;
	instance_offset 	= 0;
	draw_commands_offset 	= 0;

	for (const auto &[mesh, instance_count] : m_instance_counter) {
		command.index_count     = mesh->indices.size ();
		command.instance_count  = instance_count;
		command.base_index      = index_offset;
		command.base_vertex     = vertex_offset;
		command.base_instance   = instance_offset;

		index_offset    += mesh->indices .size ();
		vertex_offset   += mesh->vertices.size ();
		instance_offset += instance_count;

		glNamedBufferSubData (
			m_dibo, 
			draw_commands_offset,
			sizeof (EngineDetail::DrawCommand),
			(void *) &command
		);

		draw_commands_offset += sizeof (EngineDetail::DrawCommand);
	}
}

void Engine::Batch::UploadInstanceDataSSBO () const {
	intptr_t offset;


	offset = 0;
	
	for (const auto &obj : m_loaded_objects) {
		if (!obj->m_should_render) continue;

		glNamedBufferSubData (
			m_instance_ssbo,
			offset,
			sizeof (EngineDetail::InstanceData),
			(void*) &obj->m_data
		);
		offset += sizeof (EngineDetail::InstanceData);
	}
}