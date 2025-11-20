#pragma once

#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <stdint.h>

#include <glm/glm.hpp>

#include <draw_command.hpp>
#include <engine_types.hpp>
#include <geometry.hpp>
#include <mesh.hpp>
#include <resource_manager.hpp>
#include <shader.hpp>
#include <texture.hpp>
#include <vertex.hpp>

namespace ED {
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
		
		void addMesh (Mesh * mesh);
		void removeMesh (Mesh * mesh);

		void syncInstances ();
		void syncCommands ();

		~Batch ();

		Batch (const Batch & other) = delete;
		Batch (Batch & other) = delete;
		Batch & operator= (const Batch & other) = delete;
		Batch & operator= (Batch & other) = delete;

		Batch (Batch && other) = default;
		Batch & operator= (Batch && other) = default;

	private:
		/* MEMBERS */

		struct GeometryEntry {
			Geometry * 	origin;
			size_t		count;
			bool		dirty;
		};

		struct MeshEntry {
			Mesh * 	origin;
			bool	dirty;
		};

		uint32_t	m_vao;
		uint32_t	m_vbo;
		uint32_t	m_ebo;
		uint32_t	m_dibo;
		uint32_t	m_instance_ssbo;

		std::unordered_map<std::string, uint32_t>	m_texture_map_handles;	// texture name in shader code -> opengl id of the object handle
		std::unordered_map<ShaderType, uint32_t>	m_shaders_handles;	// shader role -> opengl id of the object handle

		std::unordered_map<Geometry *, size_t>		m_geometry_indices;
		std::vector<GeometryEntry>			m_geometry_entries;
		std::vector<DrawCommand>			m_commands;
		
		std::unordered_map<Mesh *, size_t>		m_mesh_indices;
		std::vector<MeshEntry>				m_mesh_entries;
		std::vector<InstanceData>			m_instances;

		uint32_t	m_vertex_buffer_size;
		uint32_t	m_index_buffer_size;
		uint32_t	m_mesh_count;


		/* METHODS */

		void createInstance	(Mesh * mesh);
		void removeInstance	(Mesh * mesh);

		void createGeometry	(Geometry * geometry);
		void increaseGeometry	(Geometry * geometry);
		void decreaseGeometry	(Geometry * geometry);
		void removeGeometry	(Geometry * geometry);

		//void updateTransformationUpload (Mesh * mesh);
		//void updateTransformation (Mesh * mesh);


		// void compileShaders ();
		// void recompileShader (const ShaderType && type);

		friend class Engine::Scene;
	};
}