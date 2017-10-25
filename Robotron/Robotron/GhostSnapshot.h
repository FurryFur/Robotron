#pragma once

#include "PhysicsComponent.h"

#include <glm\glm.hpp>

class OutBufferStream;

struct GhostSnapshot {
	GhostSnapshot() = default;
	GhostSnapshot(const glm::mat4& transform, const PhysicsComponent&);

	glm::mat4 transform;
	PhysicsComponent physics;

	OutBufferStream& serialize(OutBufferStream&) const;
};

OutBufferStream& operator<<(OutBufferStream&, const GhostSnapshot&);