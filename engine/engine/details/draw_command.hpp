#pragma once

#include <stdint.h>

namespace EngineDetail {
	struct DrawCommand {
		uint32_t index_count;
		uint32_t instance_count;
		uint32_t base_index;
		int32_t  base_vertex;
		uint32_t base_instance;
	};
}