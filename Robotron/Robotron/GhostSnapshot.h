#pragma once

#include "PhysicsComponent.h"

#include <glm\glm.hpp>

#include <cstdint>

class OutBufferStream;
class InBufferStream;

struct GhostSnapshot {
	GhostSnapshot() = default;
	GhostSnapshot(std::int32_t entityNetId, const glm::mat4& transform, const PhysicsComponent&);

	std::int32_t entityNetId;
	glm::mat4 transform;
	PhysicsComponent physics;

	OutBufferStream& serialize(OutBufferStream&) const;
	InBufferStream& deserialize(InBufferStream&);
};

OutBufferStream& operator<<(OutBufferStream&, const GhostSnapshot&);
InBufferStream& operator>>(InBufferStream&, GhostSnapshot&);