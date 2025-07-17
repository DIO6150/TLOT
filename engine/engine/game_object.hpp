#pragma once

#include <glm/glm.hpp>

#include <details/instance_data.hpp>

namespace EngineDetail {
	class Batch;
}

namespace Engine {
	class Scene;
	class Mesh;

	class GameObject {
	private:
		Mesh *				m_mesh;
		EngineDetail::InstanceData 	m_data;
		
		Scene *				m_parent_scene;
		EngineDetail::Batch *		m_parent_batch;

		bool 				m_should_render;

		friend 				Scene;
		friend 				EngineDetail::Batch;

		GameObject (
			Mesh *				mesh,
			EngineDetail::InstanceData 	data,
			Scene *				scene,
			EngineDetail::Batch *		batch
		);

	public:
		GameObject () {}

		void Translate (glm::vec3 translation);
		
		void Show ();
		void Hide ();
	};
}