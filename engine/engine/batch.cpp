#include <batch.hpp>
#include <draw_command.hpp>
#include <instance_data.hpp>

#include <glad/glad.h>

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
	glNamedBufferData (m_instance_ssbo, MAX_INSTANCE_COUNT * sizeof (ED::InstanceData), NULL, GL_DYNAMIC_DRAW);

	unbind ();
}

ED::Batch & ED::Batch::attachShader (Shader & shader) {

}

ED::Batch & ED::Batch::attachTexture (Texture & texture) {

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

void ED::Batch::addMesh (Mesh & mesh) {
	Geometry * geometry = mesh.geometry;

	if (!m_indices.count (geometry)) {

		m_indices	.insert ({geometry, m_instances.size () });
		m_instances	.push_back (geometry);
		m_meshes	.insert ({geometry, {&mesh}});

		createDrawCommand (geometry);

		return;
	}

	m_meshes.at (geometry).insert (&mesh);

	increaseDrawCommmand (geometry);
}

void ED::Batch::syncCommands () {
	size_t i = 0;

	for (DrawCommandCPU & cpu_command : m_commands_mirror) {
		++i;

		if (!cpu_command.dirty) continue;

		glNamedBufferSubData (
			m_dibo, 
			i * sizeof (DrawCommand),
			sizeof (DrawCommand),
			(void *) &cpu_command.command
		);
	}
}

void ED::Batch::syncInstances () {
	intptr_t offset = 0;
	
	for (const auto & geometry : m_instances) {
		for (const auto & mesh : m_meshes.at (geometry)) {	
			glNamedBufferSubData (
				m_instance_ssbo,
				offset,
				sizeof (InstanceData),
				(void*) &mesh->data
			);

			offset += sizeof (InstanceData);
		}
	}
}

void ED::Batch::removeMesh (Mesh & mesh) {
	m_meshes.at (mesh.geometry).erase (&mesh);
	
	decreaseDrawcommand (mesh.geometry);
}

void ED::Batch::removeGeometry (Geometry * geometry) {
	size_t idx = m_indices.at (geometry);

	Geometry *& last = m_instances.back ();
	DrawCommandCPU & last_command = m_commands_mirror.back ();
	
	if (last != geometry) {
		std::swap (m_instances.at (idx), last);
		std::swap (m_commands_mirror.at (idx), last_command);
		m_indices.at (last) = idx;
	}
	
	m_instances.pop_back ();
	m_commands_mirror.pop_back ();
	m_indices.erase (geometry);
	m_meshes.erase (geometry);

	removeDrawCommands (idx);
}

void ED::Batch::createDrawCommand (Geometry * geometry) {
	uint32_t instance_count = m_meshes.at (geometry).size ();
	const DrawCommand & last = m_commands_mirror.back ().command;

	DrawCommand command;
	command.index_count     = geometry->indices.size ();
	command.instance_count  = instance_count;
	command.base_index      = geometry->indices .size () + last.base_index;
	command.base_vertex     = geometry->vertices.size () + last.base_vertex;
	command.base_instance   = instance_count + last.base_instance;

	m_commands_mirror.push_back ({command, true});
}

void ED::Batch::increaseDrawCommmand (Geometry * geometry) {
	size_t idx = m_indices.at (geometry);
	
	for (size_t i = idx; i < m_commands_mirror.size (); ++i) {
		m_commands_mirror[i].command.base_instance++;
		m_commands_mirror[i].dirty = true;
	}
}

void ED::Batch::decreaseDrawcommand (Geometry * geometry) {
	size_t idx = m_indices.at (geometry);
	
	for (size_t i = idx; i < m_commands_mirror.size (); ++i) {
		m_commands_mirror[i].command.base_instance--;
		m_commands_mirror[i].dirty = true;
	}
}

void ED::Batch::removeDrawCommands (size_t from) {
	uint32_t base_vertex 	= 0;
	uint32_t base_index 	= 0;
	uint32_t base_instance 	= 0;

	if (from != 0) {
		base_vertex 	= m_commands_mirror[from - 1].command.base_vertex;
		base_index 	= m_commands_mirror[from - 1].command.base_index;
		base_instance 	= m_commands_mirror[from - 1].command.base_instance;
	}

	for (int i = from; i < m_commands_mirror.size (); ++i) {
		uint32_t instance_count = m_meshes.at (m_instances[i]).size ();
		m_commands_mirror[i].command.base_index      = base_index;
		m_commands_mirror[i].command.base_vertex     = base_vertex;
		m_commands_mirror[i].command.base_instance   = base_instance;

		base_index    += m_instances[i]->indices .size ();
		base_vertex   += m_instances[i]->vertices.size ();
		base_instance += instance_count;

		m_commands_mirror[i].dirty = true;
	}
}

ED::Batch::~Batch () {
	glDeleteVertexArrays 	(1, &m_vao);
	glDeleteBuffers 	(1, &m_vbo);
	glDeleteBuffers 	(1, &m_ebo);
	glDeleteBuffers 	(1, &m_dibo);
	glDeleteBuffers 	(1, &m_instance_ssbo);
}


ED::Batch::Batch (Batch && other) {
	m_vao 		= other.m_vao;
	m_vbo 		= other.m_vbo;
	m_ebo 		= other.m_ebo;
	m_dibo 		= other.m_dibo;
	m_instance_ssbo = other.m_instance_ssbo;

	m_texture_map_handles	.swap (other.m_texture_map_handles);
	m_shaders_handles	.swap (other.m_shaders_handles);

	m_vertex_buffer_size 	= other.m_vertex_buffer_size;
	m_index_buffer_size 	= other.m_index_buffer_size;
	m_mesh_count 		= other.m_mesh_count;

	other.m_vao 		= 0;
	other.m_vbo 		= 0;
	other.m_ebo 		= 0;
	other.m_dibo 		= 0;
	other.m_instance_ssbo 	= 0;
}

ED::Batch & ED::Batch::operator= (Batch && other) {
	m_vao 		= other.m_vao;
	m_vbo 		= other.m_vbo;
	m_ebo 		= other.m_ebo;
	m_dibo 		= other.m_dibo;
	m_instance_ssbo = other.m_instance_ssbo;

	m_texture_map_handles	.swap (other.m_texture_map_handles);
	m_shaders_handles	.swap (other.m_shaders_handles);

	m_vertex_buffer_size 	= other.m_vertex_buffer_size;
	m_index_buffer_size 	= other.m_index_buffer_size;
	m_mesh_count 		= other.m_mesh_count;

	other.m_vao 		= 0;
	other.m_vbo 		= 0;
	other.m_ebo 		= 0;
	other.m_dibo 		= 0;
	other.m_instance_ssbo 	= 0;

	return (*this);
}