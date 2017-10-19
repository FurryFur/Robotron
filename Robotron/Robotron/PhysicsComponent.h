#pragma once

#include <glm\glm.hpp>

struct PhysicsComponent {
	glm::vec3 velocity;
	//Used to find the last direction the object was travelling before they were stationary.
	glm::vec3 previousVelocity;
	glm::vec3 acceleration;
};