#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace TetoEngine
{
	struct Transform
	{
		glm::vec3 position =  {0.0, 0.0, 0.0};
		glm::quat rotation =  {1.0, 0.0, 0.0, 0.0};
		glm::vec3 pivot    =  {0.0, 0.0, 0.0};
		glm::vec3 scale    =  {1.0, 1.0, 1.0};

		glm::mat4 model() const;
	};
	
}