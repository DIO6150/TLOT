#pragma once

#include <glm/glm.hpp>

#include <details/instance_data.hpp>

namespace Engine {
	class Scene;
	class Batch;
	class Mesh;

	class GameObject {
	private:
		Mesh *		m_mesh;
		EngineDetail::InstanceData 	m_data;
		
		Scene *		m_parent_scene;
		Batch *		m_parent_batch;

		bool 		m_should_render;

		friend 		Scene;
		friend 		Batch;

		GameObject (
			Mesh *				mesh,
			EngineDetail::InstanceData 	data,
			Scene *				scene,
			Batch *				batch
		);

	public:
		GameObject () {}

		void Translate (glm::vec3 translation);
		
		void Show ();
		void Hide ();
	};
}