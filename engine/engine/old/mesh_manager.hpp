#pragma once

#include <vector>

#include "mesh.hpp"

namespace Engine {
	class MeshManager {
	private:
		std::vector<Mesh> m_loaded_meshes;
	public:
		MeshManager ();

		Mesh *CreateMesh (
			std::vector<Vertex> vertices,
			std::vector<uint32_t> indices
		);

		void Cleanup ();
	};
}