#pragma once

#include <vector>
#include <unordered_map>
#include <stdint.h>

#include <shader.hpp>

#define MAX_INSTANCE_COUNT 65536

namespace Engine {
	class Mesh;
	class Layout;
	class Scene;
	class GameObject;
}

namespace EngineDetail
{
	class Batch {
	private:
		std::unordered_map<Engine::Mesh *, uint32_t>	m_instance_counter;
		std::vector<Engine::GameObject *>		m_loaded_objects;
		
		bool 						m_should_resend_ssbo;
		uint32_t					m_visible_mesh_count;
		uint32_t					m_vertex_count;
		uint32_t					m_index_count;

		uint32_t					m_vao;
		uint32_t					m_vbo;
		uint32_t					m_ebo;
		uint32_t					m_dibo;
		uint32_t					m_instance_ssbo;

		uint32_t					m_diffuseMap;
		uint32_t					m_ambientMap;
		uint32_t					m_normalMap;
		uint32_t					m_specularMap;
		uint32_t					m_albedoMap;

		Engine::Shader *				m_shader;
		Engine::Layout *				m_layout;

		Engine::Scene  *				m_parent_scene;

		uint32_t					m_hash;

		friend 					Engine::Scene;
		friend					Engine::GameObject;


		void UploadMesh (Engine::Mesh *mesh);
		void Expand 	(Engine::Mesh *target_mesh);

	public:
		Batch &operator= (const Batch other);
		Batch (const Batch &other);
		Batch ();
		Batch (Engine::Shader* shader_base);
		Batch (Engine::Layout *&&layout, Engine::Shader* shader_base);
		Batch (Engine::Layout *&&layout, std::vector<Engine::Mesh *> &meshes, Engine::Shader* shader_base);
		~Batch ();

		void Bind () const;
		void Unbind () const;
		
		void UploadGameObject (Engine::GameObject *object);

		Engine::Shader *GetShader () const;

		uint32_t GetDiffuse () const;
		uint32_t GetAmbient () const;

		uint32_t GetHash () const; // UNUSED : maybe remove it ?

		void UploadDrawCommands () const;
		void UploadInstanceDataSSBO () const;
	};
}