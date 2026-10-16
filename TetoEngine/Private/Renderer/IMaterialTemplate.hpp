#pragma once

#include <string>
#include <vector>

#include <TetoEngine/Common/Uniform.hpp>

namespace TetoEngine
{
	class IMaterialTemplate
	{
	protected:
		virtual size_t getSize(UniformType type) = 0;
		virtual size_t getAlignment(UniformType type) = 0;
	};	
}
