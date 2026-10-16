#pragma once

#include <stdint.h>

#include <vector>

namespace TetoEngine
{
	using ID64 = uint64_t;

	static ID64 const InvalidID = UINT64_MAX;

	ID64 hash_combine(ID64 seed, ID64 value);
	ID64 generate_ordered_set_hash(std::vector<ID64> const & handles);
}
