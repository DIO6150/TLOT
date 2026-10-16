#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ostream>

namespace TetoEngine
{
	class Camera
	{
	public:
		Camera ();

		void move(glm::vec3 offset);

		void moveForward (float deltaTime);
		void moveBackward(float deltaTime);
		void moveLeft    (float deltaTime);
		void moveRight   (float deltaTime);
		void moveUp      (float deltaTime);
		void moveDown    (float deltaTime);

		void rotate(glm::vec3 offset);

		glm::mat4 view();

	private:
		glm::vec3 position_;
		glm::vec3 direction_;
		glm::vec3 rotation_; // yaw, pitch and roll
		glm::vec3 up_;
		float speed_;
	};
}
