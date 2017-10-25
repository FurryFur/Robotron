#include "PhysicsComponent.h"

#include "BufferStream.h"
#include "GLMUtils.h"

OutBufferStream& PhysicsComponent::serialize(OutBufferStream& obs) const
{
	return obs << velocity << acceleration;
}

InBufferStream& PhysicsComponent::deserialize(InBufferStream& ibs)
{
	return ibs >> velocity >> acceleration;
}

OutBufferStream& operator<<(OutBufferStream& obs, const PhysicsComponent& physics)
{
	return physics.serialize(obs);
}

InBufferStream& operator>>(InBufferStream& ibs, PhysicsComponent& physics)
{
	return physics.deserialize(ibs);
}
