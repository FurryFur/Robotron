#include "GhostSnapshot.h"

#include "GLMUtils.h"
#include "PhysicsComponent.h"
#include "BufferStream.h"

GhostSnapshot::GhostSnapshot(std::int32_t entityNetId, const glm::mat4& transform, const PhysicsComponent& physics)
{
	this->entityNetId = entityNetId;
	this->transform = transform;
	this->physics = physics;
}

OutBufferStream& GhostSnapshot::serialize(OutBufferStream& obs) const
{
	return obs << entityNetId << transform << physics;
}

InBufferStream& GhostSnapshot::deserialize(InBufferStream& ibs)
{
	return ibs >> entityNetId >> transform >> physics;
}

OutBufferStream& operator<<(OutBufferStream& obs, const GhostSnapshot& snapshot)
{
	return snapshot.serialize(obs);
}

InBufferStream & operator>>(InBufferStream& ibs, GhostSnapshot& snapshot)
{
	return snapshot.deserialize(ibs);
}
