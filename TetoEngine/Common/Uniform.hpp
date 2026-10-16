#pragma once

#include <stdint.h>
#include <string>

namespace TetoEngine
{
	enum UniformType { Int, Float, Vec2, Vec3, Vec4, Mat4 };

	struct UniformLayout
	{
		UniformType type;
		std::string name;
		size_t      size_;
		size_t      offset;
	};
};