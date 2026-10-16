#pragma once

#include <stdint.h>
#include <vector>

namespace TetoEngine
{
    class FrameBuffer
	{
	public:
		FrameBuffer (int32_t width, int32_t height);

		void addColorAttachment ();

		uint32_t get () const;
		uint32_t getColorAttachment (uint32_t binding) const;

	private:
        std::vector<uint32_t> colorAttachment_;

		uint32_t frameBuffer_;
		uint32_t renderBuffer_;

		int32_t width_;
		int32_t height_;
	};
}
