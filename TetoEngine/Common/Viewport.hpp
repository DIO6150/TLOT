#pragma once

#include <stdint.h>

namespace TetoEngine
{
	class Viewport
	{
	public:
		Viewport(uint32_t width, uint32_t height)
			: width_  {width}
			, height_ {height}
		{}

		float AspectRatio() { return static_cast<float>(width_) / height_; }

		uint32_t width_;
		uint32_t height_;
	};
}