#pragma once

#include <stdint.h>

#include <engine/core/ResourceManager.hpp>

#include <engine/utils/Types.hpp>

#include <glm/glm.hpp>

namespace Engine::Data {
	struct Material {
		std::vector<Core::HandleID> diffuseTextures;

		glm::vec3 diffuseColor;
	};
}