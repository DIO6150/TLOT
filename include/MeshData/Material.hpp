#pragma once

#include <vector>

#include <Core/ResourceHandle.hpp>

#include <glm/glm.hpp>

namespace TLOT
{
	struct Material
	{
		glm::vec4 color = glm::vec4 (1.0);
		std::vector<ResourceHandle> diffuseTextures;
	};
}