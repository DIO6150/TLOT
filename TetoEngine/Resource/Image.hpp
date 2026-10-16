#pragma once

#include <vector>

namespace TetoEngine
{
	struct Image
	{
		std::vector<uint8_t> data;
		int32_t width    = 0;
		int32_t height   = 0;
		int32_t channels = 0;
	
	};
}
