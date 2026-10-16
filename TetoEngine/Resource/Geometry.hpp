#pragma once

#include <stdint.h>

#include <vector>

namespace TetoEngine
{
	struct Geometry
	{
		std::vector<float>     vertices_;
		std::vector<uint32_t>  indices_;
	};
}
