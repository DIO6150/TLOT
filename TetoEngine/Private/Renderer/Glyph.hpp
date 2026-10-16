#pragma once

#include <vector>

namespace TetoEngine
{
	struct Glyph
	{
		std::vector<unsigned char> bitmap_;
		int width_;
		int height;
		int bearingX_;
		int bearingY_;
		int advance_;
	};
}