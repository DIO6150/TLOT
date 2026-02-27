#pragma once

#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <stdint.h>

#include <glm/glm.hpp>

#include <engine/opengl/DrawCommand.hpp>

#include <engine/data/Geometry.hpp>
#include <engine/data/Mesh.hpp>
#include <engine/data/Shader.hpp>
#include <engine/data/Texture.hpp>
#include <engine/data/Vertex.hpp>

#include <engine/utils/Types.hpp>

namespace Engine {
	class Batch {
	public:
		Batch ();

		Batch & attachShader (Shader * shader);

		void bind ();
		void unbind ();
		
		void addMesh (Mesh * mesh);
		void removeMesh (Mesh * mesh);

		void syncInstances ();
		void syncCommands ();

		void updateInstance (Mesh * mesh);

		uint32_t getMeshCount () const;
		Shader * getShader () const;

		uint32_t getDrawCommandCount () const;

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
		
		Shader *					m_shader;

		std::unordered_map<Geometry *, size_t>		m_geometry_indices;
		std::vector<GeometryEntry>			m_geometry_entries;
		std::vector<DrawCommand>			m_commands;
		
		std::unordered_map<Mesh *, size_t>		m_mesh_indices;
		std::vector<MeshEntry>				m_mesh_entries;
		std::vector<InstanceData>			m_instances;

		uint32_t	m_vertex_count;
		uint32_t	m_index_count;

		uint32_t	m_vertex_count_max;
		uint32_t	m_index_count_max;

		uint32_t	m_mesh_count;


		/* METHODS */
		
		void generateAttributes	();
		void expand		(Geometry * target_geometry);
		bool expand_2		();

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

		friend class Scene;
	};
}