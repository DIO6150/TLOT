#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine::Data {
	struct Instance {
		glm::mat4	transform;
		glm::vec4	coords_ratio;
		glm::vec4	color;

		Instance ():
			transform	{1.0f},
			coords_ratio	{0.0f},
			color		{1.0f}
			{

		}

		Instance (glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, glm::vec4 coords_ratio, glm::vec3 color):
			transform	{1.0f},
			coords_ratio	{coords_ratio},
			color		{color, 1.0f}
			{
			transform = glm::scale	(transform, scale);
			transform = glm::rotate (transform, rot.z, glm::vec3 (0.0, 0.0, 1.0));
			transform = glm::rotate (transform, rot.y, glm::vec3 (0.0, 1.0, 0.0));
			transform = glm::rotate (transform, rot.x, glm::vec3 (1.0, 0.0, 0.0));
			transform = glm::translate (transform, pos);

		}
	};
}
