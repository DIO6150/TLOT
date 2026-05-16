#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace TLOT
{
	struct Transform
	{
		glm::vec3 position =  {0.0, 0.0, 0.0};
		glm::quat rotation =  {1.0, 0.0, 0.0, 0.0};
		glm::vec3 pivot    =  {0.0, 0.0, 0.0};
		glm::vec3 scale    =  {1.0, 1.0, 1.0};

		glm::mat4 GetModelMatrix() const
		{
			glm::mat4 model = glm::translate(glm::mat4 {1.0}, position);
			model = model * glm::mat4_cast (rotation);
			model = glm::translate (model, -pivot * scale);
			model = glm::scale (model, scale);

			return model;
		}
	};
	
}