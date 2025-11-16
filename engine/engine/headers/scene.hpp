#pragma once

#include <vector>

#include <engine_types.hpp>

namespace Engine {
	class Scene {
	public:
		Scene ();

		void addMesh (Handle mesh);
		void removeMesh (Handle mesh);
	private:
		std::vector<Mesh *> 		m_meshes;
		std::vector<ED::Batch *> 	m_batches;
	};
};