#pragma once

#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <stdint.h>

#include <glm/glm.hpp>

#include <draw_command.hpp>
#include <geometry.hpp>
#include <mesh.hpp>
#include <resource_manager.hpp>
#include <shader.hpp>
#include <texture.hpp>
#include <vertex.hpp>

namespace ED {
	struct DrawCommandCPU {
		DrawCommand command;
		bool dirty;
	};

	class Batch {
	public:
		Batch ();

		/// @brief Add a shader to the batch. Return this for chaining purposes
		/// @param shader 
		/// @return this
		Batch & attachShader (Shader & shader);
		Batch & attachTexture (Texture & texture);

		void bind ();
		void unbind ();

		void addMesh (Mesh & mesh);
		
		void syncCommands ();
		void syncInstances ();

		void removeMesh (Mesh & mesh);
		void removeGeometry (Geometry * geometry);

		void modifyMesh (Mesh * mesh);

		~Batch ();

		Batch (const Batch & other) = delete;
		Batch (Batch & other) = delete;
		Batch & operator= (const Batch & other) = delete;
		Batch & operator= (Batch & other) = delete;

		Batch (Batch && other);
		Batch & operator= (Batch && other);

	private:
		/* MEMBERS */

		uint32_t	m_vao;
		uint32_t	m_vbo;
		uint32_t	m_ebo;
		uint32_t	m_dibo;
		uint32_t	m_instance_ssbo;

		std::unordered_map<std::string, uint32_t>	m_texture_map_handles;	// texture name in shader code -> opengl id of the object handle
		std::unordered_map<ShaderType, uint32_t>	m_shaders_handles;	// shader role -> opengl id of the object handle

		std::unordered_map<Geometry *, size_t>		m_geometry_indices;
		std::vector<Geometry *>				m_geometry_instances;
		std::unordered_map<Geometry *, size_t>		m_geometry_count;
		std::vector<DrawCommandCPU>			m_commands_mirror;

		std::unordered_map<Mesh *, size_t>		m_mesh_indices;
		std::vector<InstanceData>			m_mesh_instances;
		std::vector<bool>				m_mesh_dirty;

		uint32_t	m_vertex_buffer_size;
		uint32_t	m_index_buffer_size;
		uint32_t	m_mesh_count;


		/* METHODS */

		void createDrawCommand (Geometry * geometry);
		void increaseDrawCommmand (Geometry * geometry);
		void decreaseDrawcommand (Geometry * geometry);
		void removeDrawCommands (size_t from);

		void compileShaders ();
		void recompileShader (const ShaderType && type);
	};
}