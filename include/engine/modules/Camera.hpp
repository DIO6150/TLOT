#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine::Module {
	struct Camera {
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 rotation; // yaw, pitch and roll
		glm::vec3 up;

		float speed;

		Camera () :
		position {0.0f},
		direction {0.0f},
		rotation {0.0f},
		up {0.0, 1.0, 0.0},
		speed {0.5f}
		{
			position.z  = -3.0f;
			direction.z =  1.0f;
		}

		void move (glm::vec3 offset) {
			position += offset;
		}

		void move_in_direction (glm::vec3 offset) {
			glm::vec3 right = glm::normalize(glm::cross(direction, up));
			glm::vec3 forward = glm::normalize(direction);
			position += forward * offset.z + right * offset.x + up * offset.y;
		}

		void move_forward (float deltaTime) {
			auto dir = glm::normalize (glm::vec3 (direction.x, 0, direction.z));
			position += dir * speed * deltaTime;
		}

		void move_backward (float deltaTime) {
			auto dir = glm::normalize (glm::vec3 (direction.x, 0, direction.z));
			position -= dir * speed * deltaTime;
		}

		void move_left (float deltaTime) {
			glm::vec3 right = glm::normalize(glm::cross(direction, up));
    			position -= right * speed * deltaTime;
		}

		void move_right (float deltaTime) {
			glm::vec3 right = glm::normalize(glm::cross(direction, up));
    			position += right * speed * deltaTime;
		}

		void move_up (float deltaTime) {
			move ({0.0, speed * deltaTime, 0.0});
		}

		void move_down (float deltaTime) {
			move ({0.0, -speed * deltaTime, 0.0});
		}

		void rotate (glm::vec3 offset) {
			rotation += offset;

			if(rotation.y > 89.0)  rotation.y =  89.0;
    			if(rotation.y < -89.0) rotation.y = -89.0;
		}

		glm::mat4 getView () { // TODO-fix : move direction logic elsewhere and make func const
			glm::vec3 _direction;
			_direction.x = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
			_direction.y = sin(glm::radians(rotation.y));
			_direction.z = sin(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
			direction = glm::normalize(_direction);
			return (glm::lookAt (position, position + direction, glm::vec3 {0.0, 1.0, 0.0}));
		}

		friend std::ostream& operator<< (std::ostream &out, const Camera & data) {
			out << "([" << data.position.x << ", " << data.position.y << ", " << data.position.x << "], [" <<
			data.rotation.x << ", " << data.rotation.y << ", " << data.rotation.z << "])";
			return out;
		}

	};
}