#pragma once

#include <glm\glm.hpp>

class OutBufferStream;
class InBufferStream;
struct PhysicsComponent;

struct PhysicsComponent {
	glm::vec3 velocity;
	glm::vec3 acceleration;

	OutBufferStream& serialize(OutBufferStream&) const;
	InBufferStream& deserialize(InBufferStream&);
};

OutBufferStream& operator<<(OutBufferStream&, const PhysicsComponent&);
InBufferStream& operator>>(InBufferStream&, PhysicsComponent&);