#pragma once

#include "PhysicsComponent.h"
#include "TransformComponent.h"

#include <glm\glm.hpp>

#include <cstdint>

class OutBufferStream;
class InBufferStream;

struct GhostSnapshot {
	GhostSnapshot() = default;
	GhostSnapshot(std::int32_t entityNetId, const TransformComponent& transform, const PhysicsComponent&);

	std::int32_t entityNetId;
	TransformComponent transform;
	PhysicsComponent physics;

	OutBufferStream& serialize(OutBufferStream&) const;
	InBufferStream& deserialize(InBufferStream&);
};

OutBufferStream& operator<<(OutBufferStream&, const GhostSnapshot&);
InBufferStream& operator>>(InBufferStream&, GhostSnapshot&);