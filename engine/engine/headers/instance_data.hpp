#pragma once

#include "glm/glm.hpp"

namespace ED {
	struct InstanceData {
		glm::mat4 matrix;
		glm::vec3 color;
		glm::vec4 atlasCoords; // x, y, w, h -> texCoords = (w, h) * texCoords + (x, y)
	};
}