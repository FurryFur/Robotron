#pragma once

#include <glm\glm.hpp>

class OutBufferStream;
class InBufferStream;
struct PhysicsComponent;

struct PhysicsComponent {
	glm::vec3 velocity;
	glm::vec3 acceleration;
};