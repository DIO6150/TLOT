#pragma once

#include <string>
#include <vector>

#include <stdint.h>

#include <engine/data/Vertex.hpp>

namespace Engine::Data {
	struct Geometry {
	public:
		Geometry ();
		Geometry (std::vector<Vertex> & vertices, std::vector<uint32_t> & indices);
	
		std::vector<Vertex> 	vertices;
		std::vector<uint32_t> 	indices;
	};
}