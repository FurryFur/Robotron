#include "GhostSnapshot.h"

#include "GLMUtils.h"
#include "PhysicsComponent.h"
#include "BufferStream.h"

GhostSnapshot::GhostSnapshot(const glm::mat4& transform, const PhysicsComponent& physics)
{
	this->transform = transform;
	this->physics = physics;
}

OutBufferStream& GhostSnapshot::serialize(OutBufferStream& obs) const
{
	return obs << transform << physics;
}

InBufferStream& GhostSnapshot::deserialize(InBufferStream& ibs)
{
	return ibs >> transform >> physics;
}

OutBufferStream& operator<<(OutBufferStream& obs, const GhostSnapshot& snapshot)
{
	return snapshot.serialize(obs);
}

InBufferStream & operator>>(InBufferStream& ibs, GhostSnapshot& snapshot)
{
	snapshot.deserialize(ibs);
}
