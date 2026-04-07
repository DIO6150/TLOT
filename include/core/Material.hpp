#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <resources/Resource.hpp>

namespace TLOT
{
	struct Material
	{
		glm::vec4 color = glm::vec4 (1.0);
		std::vector<ResourceHandle> diffuseTextures;
	};
}