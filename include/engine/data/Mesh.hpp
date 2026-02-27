#pragma once

#include <string>
#include <vector>

#include <stdint.h>

#include <glm/glm.hpp>

#include <engine/core/ResourceManager.hpp>
#include <engine/utils/Types.hpp>

namespace Engine::Data {
	struct Mesh {
		Core::HandleID vertices;
		Core::HandleID indices;
		Core::HandleID material;

		glm::mat4 transform;

		std::string name;
	};
}