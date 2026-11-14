#pragma once

#include <string>
#include <vector>

#include <TetoEngine/Common/ID.hpp>
#include <TetoEngine/Common/Uniform.hpp>


namespace TetoEngine
{
	struct MaterialLayout
	{
		std::pair<std::string, std::string> shader;
		std::vector<std::pair<std::string, UniformType>> shaderProperties;
		std::vector<std::pair<std::string, UniformType>> instanceProperties;
	};
}
