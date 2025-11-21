#pragma once

#include "glm/glm.hpp"

#include <vector>

namespace Engine {
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

	struct Mesh {
		std::vector<Vertex>	vertices;
		std::vector<uint32_t>	indices;
	};
}