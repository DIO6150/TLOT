#pragma once

#include <glm/glm.hpp>

namespace Engine {
	struct InstanceData {
		InstanceData ();
		InstanceData (
			glm::vec3 position, 
			glm::vec3 rotation,
			glm::vec3 scale,
			glm::vec4 coords
		);

		glm::mat4 matrix;
		glm::vec4 atlasCoords; // x, y, w, h -> texCoords = (w, h) * texCoords + (x, y)
	};
}