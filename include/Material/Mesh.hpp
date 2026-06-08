#pragma once

#include <vector>

#include <MeshData/Material.hpp>
#include <MeshData/Vertex.hpp>

#include <Internal/Hash.hpp>

namespace TLOT
{
	using Vertices_t = std::vector<Vertex>;
	using Indices_t  = std::vector<uint32_t>;

	struct Mesh
	{
		Vertices_t vertices;
		Indices_t  indices;
		Material   material;
		
		ID64_t      hash = 0;
	};
}
