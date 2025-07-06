#pragma once

#include <vector>
#include <unordered_map>
#include <stdint.h>

#include "shader.hpp"

#define MAX_INSTANCE_COUNT 65536

namespace Engine
{
	class Mesh;
	class Layout;
	class Scene;
	class GameObject;

	class Batch {
	private:
		std::unordered_map<Mesh *, uint32_t>	m_instance_counter;
		std::vector<GameObject *>		m_loaded_objects;
		
		bool 					m_should_resend_ssbo;
		uint32_t				m_visible_mesh_count;
		uint32_t				m_vertex_count;
		uint32_t				m_index_count;

		uint32_t				m_vao;
		uint32_t				m_vbo;
		uint32_t				m_ebo;
		uint32_t				m_dibo;
		uint32_t				m_instance_ssbo;

		uint32_t				m_diffuseMap;
		uint32_t				m_ambientMap;
		uint32_t				m_normalMap;
		uint32_t				m_specularMap;
		uint32_t				m_albedoMap;

		Shader *				m_shader;
		Layout *				m_layout;

		Scene *					m_parent_scene;

		uint32_t				m_hash;

		friend 					Scene;
		friend					GameObject;


		void UploadMesh (Mesh *mesh);
		void Expand (Mesh *target_mesh);

	public:
		Batch &operator= (const Batch other);
		Batch (const Batch &other);
		Batch ();
		Batch (Shader* shader_base);
		Batch (Layout *&&layout, Shader* shader_base);
		Batch (Layout *&&layout, std::vector<Mesh *> &meshes, Shader* shader_base);
		~Batch ();

		void Bind () const;
		void Unbind () const;
		
		void UploadGameObject (GameObject *object);

		Shader *GetShader () const;

		uint32_t GetDiffuse () const;
		uint32_t GetAmbient () const;

		uint32_t GetHash () const; // UNUSED : maybe remove it ?

		void UploadDrawCommands () const;
		void UploadInstanceDataSSBO () const;
	};
}