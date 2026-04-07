#pragma once

#include <core/Vertex.hpp>
#include <core/Material.hpp>

#include <vector>

namespace TLOT
{
	struct Mesh
	{
		Mesh (auto vertices, auto indices, auto material, auto identifier):
			vertices {vertices},
			indices {indices},
			material {material},
			identifier {identifier}
		{
			
		}

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		Material material;
		
		ID identifier;
	};
}
