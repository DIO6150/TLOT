#pragma once

#include "glm/glm.hpp"

#include "instance_data.hpp"

namespace Engine {
	class Scene;
	class Batch;
	class Mesh;

	class GameObject {
	private:
		Mesh *		m_mesh;
		InstanceData 	m_data;
		
		Scene *		m_parent_scene;
		Batch *		m_parent_batch;

		bool 		m_should_render;

		friend 		Scene;
		friend 		Batch;

		GameObject (
			Mesh *		mesh,
			InstanceData 	data,
			Scene *		scene,
			Batch *		batch
		);

	public:
		GameObject () {}

		void Translate (glm::vec3 translation);
		
		void Show ();
		void Hide ();
	};
}