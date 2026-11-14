#pragma once

#include <TetoEngine/Common/ID.hpp>

namespace TetoEngine
{
	using TRID = ID64; // TetoRendererID

	struct TetoInstance // Teto Instance Renderer ID
	{
		ID64 geometryID;
		ID64 materialID;
		
		// Geometry offsets
		uint32_t indexCount;
		uint32_t firstIndex;
		int32_t  baseVertex;

		// Index in SSBO
		uint32_t matrixIndex;
		uint32_t materialIndex;

		// culling flag
		bool visible_ = true; 
	};
}