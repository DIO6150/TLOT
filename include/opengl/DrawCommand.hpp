#pragma once

#include <stdint.h>

namespace TLOT
{
    struct DrawCommand {
		uint32_t indexCount;
		uint32_t instanceCount;
		uint32_t firstIndex;
		int32_t  baseVertex;
		uint32_t baseInstance;
	};
}