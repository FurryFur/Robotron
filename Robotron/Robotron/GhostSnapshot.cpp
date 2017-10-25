#include "GhostSnapshot.h"

#include "GLMUtils.h"
#include "PhysicsComponent.h"

GhostSnapshot::GhostSnapshot(const glm::mat4& transform, const PhysicsComponent& physics)
{
	this->transform = transform;
	this->physics = physics;
}

OutBufferStream& GhostSnapshot::serialize(OutBufferStream& obs) const
{
	return obs << transform << physics;
}

OutBufferStream& operator<<(OutBufferStream& obs, const GhostSnapshot& snapshot)
{
	return snapshot.serialize(obs);
}
