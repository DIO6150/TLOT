#include <TetoEngine/Common/Transform.hpp>

using namespace TetoEngine;

// TODO: check the math urgh
glm::mat4 Transform::model() const
{
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
	if (pivot != glm::vec3(0.0f)) {
		rotationMatrix = glm::translate(rotationMatrix, -pivot);
	}

	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);

	return translationMatrix * rotationMatrix * scaleMatrix;
}
