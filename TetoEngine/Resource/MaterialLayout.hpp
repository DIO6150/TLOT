#pragma once

#include <string>
#include <vector>

#include <TetoEngine/Common/ID.hpp>
#include <TetoEngine/Common/Uniform.hpp>


namespace TetoEngine
{
	struct MaterialLayout
	{
		std::vector<std::pair<std::string, UniformType>> uniforms_;
		std::vector<std::string> textures_;
	};
}
