#pragma once

#include <glm\glm.hpp>

struct PhysicsComponent {
	glm::vec3 position;
	glm::vec3 orientation;
	glm::vec3 velocity;
	glm::vec3 acceleration;
};