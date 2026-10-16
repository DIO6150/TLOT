#include <TetoEngine/Extensions/Camera.hpp>

using namespace TetoEngine;

Camera::Camera()
	: position_{0.0f}
	, direction_{0.0f}
	, rotation_{0.0f}
	, up_{0.0, 1.0, 0.0}
	, speed_{0.5f}
{}


void Camera::move(glm::vec3 offset)
{
	glm::vec3 right = glm::normalize(glm::cross(direction_, up_));
	glm::vec3 forward = glm::normalize(direction_);
	position_ += forward * offset.z + right * offset.x + up_ * offset.y;
}

void Camera::moveForward(float deltaTime)
{
	auto dir = glm::normalize(glm::vec3(direction_.x, 0, direction_.z));
	position_ += dir * speed_ * deltaTime;
}

void Camera::moveBackward(float deltaTime)
{
	auto dir = glm::normalize(glm::vec3(direction_.x, 0, direction_.z));
	position_ -= dir * speed_ * deltaTime;
}

void Camera::moveLeft(float deltaTime)
{
	glm::vec3 right = glm::normalize(glm::cross(direction_, up_));
	position_ -= right * speed_ * deltaTime;
}

void Camera::moveRight(float deltaTime)
{
	glm::vec3 right = glm::normalize(glm::cross(direction_, up_));
	position_ += right * speed_ * deltaTime;
}

void Camera::moveUp(float deltaTime)
{
	move({0.0, speed_ * deltaTime, 0.0});
}

void Camera::moveDown(float deltaTime)
{
	move({0.0, -speed_ * deltaTime, 0.0});
}

void Camera::rotate(glm::vec3 offset)
{
	rotation_ += offset;

	if(rotation_.y > 89.0)  rotation_.y =  89.0;
	if(rotation_.y < -89.0) rotation_.y = -89.0;
}

glm::mat4 Camera::view()
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(rotation_.x)) * cos(glm::radians(rotation_.y));
	direction.y = sin(glm::radians(rotation_.y));
	direction.z = sin(glm::radians(rotation_.x)) * cos(glm::radians(rotation_.y));

	direction_ = glm::normalize(direction);
	return(glm::lookAt(position_, position_ + direction_, glm::vec3 {0.0, 1.0, 0.0}));
}
