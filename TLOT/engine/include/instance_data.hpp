#pragma once

#include <glm/glm.hpp>

namespace ED {
	struct InstanceData {
		InstanceData ();
		InstanceData (
			glm::vec3 position, 
			glm::vec3 rotation,
			glm::vec3 scale
		);

		glm::mat4 matrix;
		glm::vec4 atlasCoords; // x, y, w, h -> texCoords = (w, h) * texCoords + (x, y)
	};
}