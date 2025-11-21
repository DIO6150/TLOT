#include <batch.hpp>
#include <draw_command.hpp>
#include <instance_data.hpp>

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

#define MAX_INSTANCE_COUNT 65536

ED::Batch::Batch () {
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
	dibo_size	= MAX_INSTANCE_COUNT * sizeof (ED::DrawCommand);

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

	bind ();

	glNamedBufferData (m_vbo,   vbo_size,  NULL, GL_DYNAMIC_DRAW);
	glNamedBufferData (m_ebo,   ebo_size,  NULL, GL_DYNAMIC_DRAW);
	glNamedBufferData (m_dibo,  dibo_size, NULL, GL_DYNAMIC_DRAW);

	glBindBuffer (GL_SHADER_STORAGE_BUFFER, m_instance_ssbo);
	glNamedBufferData (m_instance_ssbo, MAX_INSTANCE_COUNT * sizeof (InstanceData), NULL, GL_DYNAMIC_DRAW);

	unbind ();

	m_instances.reserve (MAX_INSTANCE_COUNT);
}

ED::Batch & ED::Batch::attachShader (__attribute__ ((unused)) Shader & shader) {
	// TODO: fix attachShader
	return (*this);
}

ED::Batch & ED::Batch::attachTexture (__attribute__ ((unused)) Texture & texture) {
	// TODO: fix attachTexture
	return (*this);
}

void ED::Batch::bind () {
	glBindVertexArray (m_vao);
	glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBindBuffer (GL_DRAW_INDIRECT_BUFFER, m_dibo);
	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, 0, m_instance_ssbo);
}

void ED::Batch::unbind () {
	glBindVertexArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer (GL_DRAW_INDIRECT_BUFFER, 0);
	glBindBuffer (GL_SHADER_STORAGE_BUFFER, 0);
}

void ED::Batch::addMesh (Mesh * mesh) {
	if (m_geometry_indices.find (mesh->geometry) == m_geometry_indices.end ()) {
		createGeometry (mesh->geometry);
		createInstance (mesh);
		
		return;
	}

	createInstance (mesh);
}

void ED::Batch::removeMesh (Mesh * mesh) {
	removeInstance (mesh);
}

void ED::Batch::syncInstances () {
	size_t range_begin = 0;
	size_t range_count = 0;
	size_t index = 0;

	for (const auto & [mesh, dirty] : m_mesh_entries) {
		if (dirty) {
			if (range_count == 0) {
				range_begin = index;
			}
			++range_count;
		}
		else if (range_count > 0) {
			glNamedBufferSubData (
				m_instance_ssbo,
				range_begin * sizeof (InstanceData),
				range_count * sizeof (InstanceData),
				(void*) m_instances.data ()
			);

			range_begin = 0;
			range_count = 0;
		}

		++index;
	}
}

void ED::Batch::syncCommands () {
	size_t range_begin = 0;
	size_t range_count = 0;
	size_t index = 0;

	for (const auto & [geometry, count, dirty] : m_geometry_entries) {
		if (dirty) {
			if (range_count == 0) {
				range_begin = index;
			}
			++range_count;
		}
		else if (range_count > 0) {

			glNamedBufferSubData (
				m_dibo, 
				range_begin * sizeof (DrawCommand),
				range_count * sizeof (DrawCommand),
				(void *) m_commands.data ()
			);

			range_begin = 0;
			range_count = 0;
		}

		++index;
	}
}

/*
void ED::Batch::updateTransformation (Mesh * mesh) {
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

void ED::Batch::updateTransformationUpload (Mesh * mesh) {
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

void ED::Batch::createInstance (Mesh * mesh) {
	m_mesh_indices.try_emplace (mesh, m_instances.size ());

	m_mesh_entries.emplace_back (mesh, true);

	m_instances.emplace_back (
		mesh->position,
		mesh->rotation,
		mesh->scale
	);

	increaseGeometry (mesh->geometry);
}

void ED::Batch::removeInstance (Mesh * mesh) {
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

void ED::Batch::createGeometry (Geometry * geometry) {
	uint32_t	base_index;
	uint32_t	base_vertex;
	uint32_t	base_instance;
	
	if (m_commands.empty ()) {
		base_index 	= 0;
		base_vertex 	= 0;
		base_instance 	= 0;
	}
	else {
		const DrawCommand & last = m_commands.back ();
		base_index 	= last.base_index;
		base_vertex 	= last.base_vertex;
		base_instance 	= last.base_instance;

	}

	m_geometry_indices.try_emplace (geometry, m_commands.size ());

	m_geometry_entries.emplace_back (geometry, 0, true);

	m_commands.emplace_back (
		geometry->indices.size (),
		0,
		geometry->indices .size () + base_index,
		geometry->vertices.size () + base_vertex,
		base_instance
	);
}

void ED::Batch::increaseGeometry (Geometry * geometry) {
	size_t idx = m_geometry_indices.at (geometry);

	++m_geometry_entries[idx].count;
	
	for (size_t i = idx; i < m_commands.size (); ++i) {
		m_geometry_entries[i].dirty = true;
		++m_commands[i].instance_count;
	}
}

void ED::Batch::decreaseGeometry (Geometry * geometry) {
	size_t idx = m_geometry_indices.at (geometry);

	--m_geometry_entries[idx].count;
	
	for (size_t i = idx; i < m_commands.size (); ++i) {
		m_geometry_entries[i].dirty = true;
		--m_commands[i].instance_count;
	}
}

void ED::Batch::removeGeometry (Geometry * geometry) {
	// TODO: fix removeGeometry
	printf ("%p", geometry);
}

ED::Batch::~Batch () {
	glDeleteVertexArrays 	(1, &m_vao);
	glDeleteBuffers 	(1, &m_vbo);
	glDeleteBuffers 	(1, &m_ebo);
	glDeleteBuffers 	(1, &m_dibo);
	glDeleteBuffers 	(1, &m_instance_ssbo);
}