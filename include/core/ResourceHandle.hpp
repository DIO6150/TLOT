#pragma once

#include <Core/ID.hpp>

namespace TLOT
{
	using ResourceHandle = ID64_t;
	static ResourceHandle const InvalidResource = InvalidID64_t;

	inline ResourceHandle GenerateHandle ()
	{
		static ResourceHandle nextHandle = 0;

		return nextHandle++;
	}
}
