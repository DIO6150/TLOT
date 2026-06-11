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
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

			glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
			if (pivot != glm::vec3(0.0f)) {
				rotationMatrix = glm::translate(rotationMatrix, -pivot);
			}

			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);

			return translationMatrix * rotationMatrix * scaleMatrix;
		}
	};
	
}