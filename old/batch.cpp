#include <engine/batch.hpp>
#include <engine/draw_command.hpp>
#include <engine/instance_data.hpp>
#include <engine/material.hpp>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

#define MAX_INSTANCE_COUNT 65536U				// totally arbitrary (2^16 is sure a nice number)
#define MAX_VERTEX_COUNT 2272727U				// ~=100Mo of memory
#define MIN_VERTEX_COUNT 36000U
#define MIN_INDEX_COUNT 150000U					//

Engine::Batch::Batch () :	m_vao {0},
			m_vbo {0},
			m_ebo {0},
			m_dibo {0},
			m_instance_ssbo {0},
			m_vertex_count {0},
			m_index_count {0},
			m_vertex_count_max {0},
			m_index_count_max {0},
			m_mesh_count {0} {
	size_t vbo_size;
	size_t ebo_size;
	size_t dibo_size;

	glGenVertexArrays(1, &m_vao);

	glGenBuffers (1, &m_vbo);
	glGenBuffers (1, &m_ebo);
	glGenBuffers (1, &m_dibo);

	glGenBuffers (1, &m_instance_ssbo);


	vbo_size	= MIN_VERTEX_COUNT 	* sizeof (Vertex);
	ebo_size	= MIN_INDEX_COUNT 	* sizeof (uint32_t);
	dibo_size	= MAX_INSTANCE_COUNT 	* sizeof (DrawCommand);

	generateAttributes ();

	bind ();

	glNamedBufferData (m_vbo,   vbo_size,  NULL, GL_DYNAMIC_DRAW);
	glNamedBufferData (m_ebo,   ebo_size,  NULL, GL_DYNAMIC_DRAW);
	glNamedBufferData (m_dibo,  dibo_size, NULL, GL_DYNAMIC_DRAW);

	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, m_instance_ssbo);
	glNamedBufferData (m_instance_ssbo, MAX_INSTANCE_COUNT * sizeof (InstanceData), NULL, GL_DYNAMIC_DRAW);

	unbind ();

	m_instances.reserve (MAX_INSTANCE_COUNT);

	m_vertex_count_max = MIN_VERTEX_COUNT;
	m_index_count_max = MIN_INDEX_COUNT;
}

Engine::Batch & Engine::Batch::attachShader (Shader * shader) {
	m_shader = shader;

	return (*this);
}

void Engine::Batch::bind () {
	glBindVertexArray (m_vao);
	glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBindBuffer (GL_DRAW_INDIRECT_BUFFER, m_dibo);
	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, m_instance_ssbo);
}

void Engine::Batch::unbind () {
	glBindVertexArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer (GL_DRAW_INDIRECT_BUFFER, 0);
	glBindBuffer (GL_SHADER_STORAGE_BUFFER, 0);
}

void Engine::Batch::addMesh (Mesh * mesh) {
	if (m_geometry_indices.find (mesh->geometry) == m_geometry_indices.end ()) {
		createGeometry (mesh->geometry);
	}

	createInstance (mesh);
}

void Engine::Batch::removeMesh (Mesh * mesh) {
	removeInstance (mesh);
}

void Engine::Batch::syncInstances () {
	size_t range_begin = 0;
	size_t range_count = 0;
	size_t index = 0;

	
	for (auto & [mesh, dirty] : m_mesh_entries) {
		//printf ("DEBUG: syncInstances exectued\n");
		if (dirty) {
			if (range_count == 0) {
				range_begin = index;
			}
			++range_count;

			dirty = false;
		}
		else if (range_count > 0) {
			glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, m_instance_ssbo);
			glNamedBufferSubData (
				m_instance_ssbo,
				range_begin * sizeof (InstanceData),
				range_count * sizeof (InstanceData),
				(void*) (m_instances.data () + range_begin)
			);

			range_begin = 0;
			range_count = 0;
		}

		++index;
	}

	if (range_count > 0) {
		glNamedBufferSubData (
			m_instance_ssbo,
			range_begin * sizeof (InstanceData),
			range_count * sizeof (InstanceData),
			(void*) (m_instances.data () + range_begin)
		);
	}
}

void Engine::Batch::syncCommands () {
	size_t range_begin = 0;
	size_t range_count = 0;
	size_t index = 0;

	
	for (auto & [geometry, count, dirty] : m_geometry_entries) {
		if (dirty) {
			if (range_count == 0) {
				range_begin = index;
			}
			++range_count;

			dirty = false;
		}
		else if (range_count > 0) {

			glNamedBufferSubData (
				m_dibo, 
				range_begin * sizeof (DrawCommand),
				range_count * sizeof (DrawCommand),
				(void*) (m_commands.data () + range_begin)
			);

			range_begin = 0;
			range_count = 0;
		}

		++index;
	}

	if (range_count > 0) {

		glNamedBufferSubData (
			m_dibo, 
			range_begin * sizeof (DrawCommand),
			range_count * sizeof (DrawCommand),
			(void*) (m_commands.data () + range_begin)
		);

		range_begin = 0;
		range_count = 0;
	}
}

void Engine::Batch::updateInstance (Mesh * mesh) {
	uint32_t idx = m_mesh_indices[mesh];
	m_mesh_entries[idx].dirty = true;

	TextureQuad * quad = mesh->material->diffuse;
	float max_size = (float) (quad->atlas->getMaxSize ());
	float x = quad->x      / max_size;
	float y = quad->y      / max_size;
	float width  = quad->w / max_size;
	float height = quad->h / max_size;
	glm::vec4 coords_ratio = { x, y, width, height };

	InstanceData new_instance_data {
		mesh->m_position,
		mesh->m_rotation,
		mesh->m_scale,
		coords_ratio
	};

	std::swap (m_instances[idx], new_instance_data);
}

/*
void Engine::Batch::updateTransformation (Mesh * mesh) {
	uint32_t idx = m_mesh_indices.at (mesh);
	InstanceData & data = m_mesh_instances.at (idx);

	data.matrix = glm::mat4 (1.0f);
	data.matrix = glm::translate (data.matrix, mesh->position);
	data.matrix = glm::rotate (data.matrix, mesh->rotation.z, glm::vec3 (0.0, 0.0, 1.0));
	data.matrix = glm::rotate (data.matrix, mesh->rotation.y, glm::vec3 (0.0, 1.0, 0.0));
	data.matrix = glm::rotate (data.matrix, mesh->rotation.x, glm::vec3 (1.0, 0.0, 0.0));
	data.matrix = glm::scale (data.matrix, mesh->position);

	m_mesh_dirty[idx] = true;
}

void Engine::Batch::updateTransformationUpload (Mesh * mesh) {
	uint32_t idx = m_mesh_indices.at (mesh);
	InstanceData & data = m_mesh_instances.at (idx);

	data.matrix = glm::mat4 (1.0f);
	data.matrix = glm::translate (data.matrix, mesh->position);
	data.matrix = glm::rotate (data.matrix, mesh->rotation.z, glm::vec3 (0.0, 0.0, 1.0));
	data.matrix = glm::rotate (data.matrix, mesh->rotation.y, glm::vec3 (0.0, 1.0, 0.0));
	data.matrix = glm::rotate (data.matrix, mesh->rotation.x, glm::vec3 (1.0, 0.0, 0.0));
	data.matrix = glm::scale (data.matrix, mesh->position);

	glNamedBufferSubData (
		m_instance_ssbo,
		sizeof (InstanceData) * idx,
		sizeof (glm::mat4),
		(void*) m_mesh_instances.data ()
	);

	// TODO : to the thing below maybe
	// should create a flag for each individual InstanceData components ?
	// rather than packing everything together maybe get rid of InstanceData
	// and create a SSBO for each component
	// that way everything is still tightly packed and we can proceed
	// with dirty flag for each ssbo without breaking linearity
	m_mesh_dirty[idx] = true;
}
*/

void Engine::Batch::generateAttributes () {
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

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

// TODO-fix : currently this function is problematic, it can provoke a TDR if used too much on the same frame.
// Problem delayed by reserving more space at the creation of the batch and allocating more memory than demanded down there.
// However it is only a temporary fix and nothing says what could happen while multiple batches are uploading on the same frame.
// Multiple fixes possible: async function that expand after some frames to be sure to not bottleneck the gpu or use another method 
// without recreating the buffer all over again
// use fences to not corrupt gpu memory while rendering
// Im gonna be honnest, I have no idea if this is really the core of the problem but anyway :(
/*
void Engine::Batch::expand (Geometry * target_geometry) {
	if (m_vertex_count_max > target_geometry->vertices.size () + m_vertex_count &&
		m_index_count_max > target_geometry->indices.size () + m_index_count) {
		//printf ("WARNING: Trying to increase the size to accommodate geometry while the buffer can perfectly handle its size, discarding.\n");
		return;
	}

	if (target_geometry->vertices.size () + m_vertex_count > MAX_VERTEX_COUNT) {
		printf ("ERROR: can't expand object buffer size further, max vert count reached.\n");
		return;
	}

	uint32_t new_vertex_count = m_vertex_count + target_geometry->vertices.size () + 30000; //padding to delay next expand
	uint32_t new_index_count  = m_index_count  + target_geometry->indices .size () + 90000;

	// Créer nouveaux buffers
	GLuint new_vbo = 0, new_ebo = 0;
	glCreateBuffers (1, &new_vbo);
	glCreateBuffers (1, &new_ebo);

	glNamedBufferData (new_vbo, new_vertex_count * sizeof(Vertex),   nullptr, GL_DYNAMIC_DRAW);
	glNamedBufferData (new_ebo, new_index_count  * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

	// Copier l’ancien contenu si nécessaire
	if (m_vertex_count > 0 && m_index_count > 0) {
		glCopyNamedBufferSubData (m_vbo, new_vbo, 0, 0, m_vertex_count * sizeof(Vertex));
		glCopyNamedBufferSubData (m_ebo, new_ebo, 0, 0, m_index_count  * sizeof(uint32_t));
	}

	// Supprimer les anciens buffers
	glDeleteBuffers (1, &m_vbo);
	glDeleteBuffers (1, &m_ebo);

	// Remplacer par les nouveaux
	m_vbo = new_vbo;
	m_ebo = new_ebo;

	generateAttributes ();

	m_vertex_count_max = new_vertex_count;
	m_index_count_max  = new_index_count;

	printf ("(DEBUG) Expanded to fit geometry [%u(vertex) %u(index)]\n", m_vertex_count_max, m_index_count_max);
}
	*/

bool Engine::Batch::expand_2 () {
	uint32_t new_vertex_count = std::min (m_vertex_count_max * 2, MAX_VERTEX_COUNT);
	uint32_t new_index_count  = std::min (m_index_count_max * 2, MAX_VERTEX_COUNT * 2);

	// Créer nouveaux buffers
	GLuint new_vbo = 0, new_ebo = 0;
	glCreateBuffers (1, &new_vbo);
	glCreateBuffers (1, &new_ebo);

	glNamedBufferData (new_vbo, new_vertex_count * sizeof(Vertex),   nullptr, GL_DYNAMIC_DRAW);
	glNamedBufferData (new_ebo, new_index_count  * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

	// Copier l’ancien contenu si nécessaire
	if (m_vertex_count > 0 && m_index_count > 0) {
		glCopyNamedBufferSubData (m_vbo, new_vbo, 0, 0, m_vertex_count * sizeof(Vertex));
		glCopyNamedBufferSubData (m_ebo, new_ebo, 0, 0, m_index_count  * sizeof(uint32_t));
	}

	// Supprimer les anciens buffers
	glDeleteBuffers (1, &m_vbo);
	glDeleteBuffers (1, &m_ebo);

	// Remplacer par les nouveaux
	m_vbo = new_vbo;
	m_ebo = new_ebo;

	generateAttributes ();

	m_vertex_count_max = new_vertex_count;
	m_index_count_max  = new_index_count;

	printf ("(DEBUG) Expanded to fit geometry [%u(vertex) %u(index)]\n", m_vertex_count_max, m_index_count_max);

	return (true);
}

void Engine::Batch::createInstance (Mesh * mesh) {
	m_mesh_indices.try_emplace (mesh, m_instances.size ());

	m_mesh_entries.emplace_back (mesh, true);

	TextureQuad * quad = mesh->material->diffuse;
	float max_size = (float) (quad->atlas->getMaxSize ());
	float x = quad->x      / max_size;
	float y = quad->y      / max_size;
	float width  = quad->w / max_size;
	float height = quad->h / max_size;
	glm::vec4 coords_ratio = { x, y, width, height };

	m_instances.emplace_back (
		mesh->m_position,
		mesh->m_rotation,
		mesh->m_scale,
		coords_ratio
	);

	increaseGeometry (mesh->geometry);
}

void Engine::Batch::removeInstance (Mesh * mesh) {
	size_t idx = m_mesh_indices.at (mesh);

	if (idx != m_mesh_indices.size () - 1) {
		std::swap (m_mesh_entries.at (idx), m_mesh_entries.back ());
		std::swap (m_instances.at (idx), m_instances.back ());
		
		m_mesh_indices.at (m_mesh_entries.at (idx).origin) = idx;
	}

	m_mesh_entries.pop_back ();
	m_instances.pop_back ();

	m_mesh_indices.erase (mesh);

	decreaseGeometry (mesh->geometry);
}

void Engine::Batch::createGeometry (Geometry * geometry) {

	if (m_vertex_count_max < geometry->vertices.size () + m_vertex_count &&
		m_index_count_max < geometry->indices.size () + m_index_count) {
		if (!expand_2 ()) return;
		if (	geometry->vertices.size () + m_vertex_count > MAX_VERTEX_COUNT ||
			geometry->indices .size () + m_index_count > MAX_VERTEX_COUNT * 2) {
			printf ("ERROR: Cant expand batch further, aborting geometry creation.\n");
			return;
		}
	}

	uint32_t base_instance;

	m_geometry_indices.try_emplace (geometry, m_commands.size ());

	m_geometry_entries.emplace_back (geometry, 0, true);

	if (m_commands.empty ()) {
		base_instance = 0U;
	}

	else {
		const DrawCommand & last_command = m_commands.back ();
		base_instance = last_command.base_instance + last_command.instance_count;
	}

	
	m_commands.emplace_back (
		geometry->indices.size (),
		0U,
		m_index_count,
		m_vertex_count,
		base_instance
	);
	
	glNamedBufferSubData (m_vbo, m_vertex_count * sizeof (Vertex),   geometry->vertices.size () * sizeof (Vertex),   geometry->vertices.data ());
	glNamedBufferSubData (m_ebo, m_index_count  * sizeof (uint32_t), geometry->indices .size () * sizeof (uint32_t), geometry->indices .data ());

	m_vertex_count += geometry->vertices.size ();
	m_index_count  += geometry->indices .size ();

}

void Engine::Batch::increaseGeometry (Geometry * geometry) {
	size_t idx = m_geometry_indices.at (geometry);

	m_geometry_entries[idx].dirty = true;

	++m_mesh_count;
	++m_geometry_entries[idx].count;
	++m_commands[idx].instance_count;
	
	for (size_t i = idx + 1; i < m_commands.size (); ++i) {
		m_geometry_entries[i].dirty = true;
		m_commands[i].base_instance = m_commands[i - 1].base_instance + m_commands[i - 1].instance_count;
	}
}

//TODO-fix: fix all decreasing and removing stuff
void Engine::Batch::decreaseGeometry (Geometry * geometry) {
	size_t idx = m_geometry_indices.at (geometry);

	--m_mesh_count;
	--m_geometry_entries[idx].count;
	
	for (size_t i = idx; i < m_commands.size (); ++i) {
		m_geometry_entries[i].dirty = true;
		--m_commands[i].instance_count;
	}
}

void Engine::Batch::removeGeometry (Geometry * geometry) {
	// TODO-fix: Remove geometry :skull:
	printf ("[Reminder to actually fix this function (removeGeometry (%p))]\n", geometry);
}

uint32_t Engine::Batch::getMeshCount () const {
	return (m_mesh_count);
}

Engine::Shader * Engine::Batch::getShader () const {
	return (m_shader);
}

uint32_t Engine::Batch::getDrawCommandCount () const {
	return (m_commands.size ());
}

Engine::Batch::~Batch () {
	glDeleteVertexArrays 	(1, &m_vao);
	glDeleteBuffers 	(1, &m_vbo);
	glDeleteBuffers 	(1, &m_ebo);
	glDeleteBuffers 	(1, &m_dibo);
	glDeleteBuffers 	(1, &m_instance_ssbo);
}