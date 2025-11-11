#pragma once

#include <string>
#include <vector>

#include <stdint.h>

#include <vertex.hpp>

namespace ED {
	class Geometry {
	public:
		Geometry ();

		std::string		name;

		std::vector<Vertex> 	vertices;
		std::vector<uint32_t> 	indices;

		// TODO : Add support for geometry hierarchy
		Geometry *		parent;
		Geometry *		children;
		size_t			child_count;
	};
}