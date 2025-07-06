#pragma once

#include "glm/glm.hpp"

#include <vector>

namespace Engine {
	class GameObject;
	class Batch;
	class Scene;
	class MeshManager;

	struct Vertex {
		float x;
		float y;
		float z;
		float u;
		float v;
		float nx;
		float ny;
		float nz;

		Vertex (
			float x,
			float y,
			float z,
			float u,
			float v,
			float nx,
			float ny,
			float nz
		);
	};

	class Mesh {
		std::vector<Vertex>	vertices;
		std::vector<uint32_t>	indices;

		friend GameObject;
		friend Batch;
		friend Scene;
		friend MeshManager;
	};
}