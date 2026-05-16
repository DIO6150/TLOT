#pragma once

#include <Core/ID.hpp>

namespace TLOT
{
	using ResourceHandle = ID_64;
	static ResourceHandle const InvalidResource = InvalidID_64;

	inline ResourceHandle GenerateHandle ()
	{
		static ResourceHandle nextHandle = 0;

		return nextHandle++;
	}
}
