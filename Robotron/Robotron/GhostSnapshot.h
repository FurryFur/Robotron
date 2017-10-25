#pragma once

#include "PhysicsComponent.h"

#include <glm\glm.hpp>

class OutBufferStream;
class InBufferStream;

struct GhostSnapshot {
	GhostSnapshot() = default;
	GhostSnapshot(const glm::mat4& transform, const PhysicsComponent&);

	glm::mat4 transform;
	PhysicsComponent physics;

	OutBufferStream& serialize(OutBufferStream&) const;
	InBufferStream& deserialize(InBufferStream&);
};

OutBufferStream& operator<<(OutBufferStream&, const GhostSnapshot&);
InBufferStream& operator>>(InBufferStream&, GhostSnapshot&);