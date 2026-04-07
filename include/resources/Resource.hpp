#pragma once

#include <stdint.h>

#include <vector>

#include <core/Utils.hpp>

namespace TLOT
{
	using ResourceHandle = uint64_t;
	using ID = uint64_t;
	using Hash = uint64_t;

	using InstanceID = uint32_t;
	using GeometryID = uint32_t;

	static ResourceHandle const InvalidResource = UINT64_MAX;
	
	static InstanceID const InvalidInstance = UINT32_MAX;
	static GeometryID const InvalidGeometry = UINT32_MAX;

	static inline Hash GenerateOrderedSetHash(const std::vector<ResourceHandle>& handles)
	{
		Hash hash = 0;

		for (ResourceHandle handle : handles)
		{
			hash = hash_combine(hash, handle);
		}

		return hash;
	}
}