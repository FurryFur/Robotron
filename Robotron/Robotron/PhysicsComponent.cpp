#include "PhysicsComponent.h"

#include "GLMUtils.h"

OutBufferStream& PhysicsComponent::serialize(OutBufferStream& obs) const
{
	return obs << velocity << acceleration;
}

OutBufferStream& operator<<(OutBufferStream& obs, const PhysicsComponent& physics)
{
	return physics.serialize(obs);
}
