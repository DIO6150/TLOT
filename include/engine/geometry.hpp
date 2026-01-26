#pragma once

#include <string>
#include <vector>

#include <stdint.h>

#include <engine/vertex.hpp>

namespace Engine {
	class Geometry {
	public:
		Geometry ();
		Geometry (std::vector<Vertex> vertices, std::vector<uint32_t> indices);

		std::vector<Vertex> 	vertices;
		std::vector<uint32_t> 	indices;

		// TODO : Add support for geometry hierarchy
		Geometry *		parent;
		Geometry *		children;
		size_t			child_count;
	};
}