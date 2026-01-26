#pragma once

#include <glm/glm.hpp>

#include <instance_data.hpp>

namespace Engine {
	class Batch;
}

namespace Engine {
	class Scene;
	class Mesh;

	class GameObject {
	private:
		Mesh *				m_mesh;
		Engine::InstanceData 	m_data;
		
		Scene *				m_parent_scene;
		Engine::Batch *		m_parent_batch;

		bool 				m_should_render;

		friend 				Scene;
		friend 				Engine::Batch;

		GameObject (
			Mesh *				mesh,
			Engine::InstanceData 	data,
			Scene *				scene,
			Engine::Batch *		batch
		);

	public:
		GameObject () {}

		void Translate (glm::vec3 translation);
		
		void Show ();
		void Hide ();
	};
}