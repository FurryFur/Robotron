#pragma once

#include <glm\glm.hpp>

class OutBufferStream;
struct PhysicsComponent;

struct PhysicsComponent {
	glm::vec3 velocity;
	glm::vec3 acceleration;

	OutBufferStream& serialize(OutBufferStream&) const;
};

OutBufferStream& operator<<(OutBufferStream&, const PhysicsComponent&);