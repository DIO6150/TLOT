#pragma once

#include <glm/glm.hpp>

#include <instance_data.hpp>

namespace ED {
	class Batch;
}

namespace Engine {
	class Scene;
	class Mesh;

	class GameObject {
	private:
		Mesh *				m_mesh;
		ED::InstanceData 	m_data;
		
		Scene *				m_parent_scene;
		ED::Batch *		m_parent_batch;

		bool 				m_should_render;

		friend 				Scene;
		friend 				ED::Batch;

		GameObject (
			Mesh *				mesh,
			ED::InstanceData 	data,
			Scene *				scene,
			ED::Batch *		batch
		);

	public:
		GameObject () {}

		void Translate (glm::vec3 translation);
		
		void Show ();
		void Hide ();
	};
}