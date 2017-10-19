#pragma once

#include <glm\glm.hpp>

struct PhysicsComponent {
	glm::vec3 velocity;
	glm::vec3 acceleration;

	// Used to find the last direction the object was travelling before they were stationary.
	glm::vec3 previousVelocity;
	// Used to store the position to wander to.
	glm::vec3 wanderPosition;
};