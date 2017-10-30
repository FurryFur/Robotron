#include "PhysicsSystem.h"

#include "RenderSystem.h"

PhysicsSystem::PhysicsSystem(Scene& scene)
	: m_scene{ scene }
{
}

void PhysicsSystem::update(Entity& entity, float deltaTick)
{
	if (!entity.hasComponents(COMPONENT_PHYSICS, COMPONENT_TRANSFORM))
		return;

	entity.physics.velocity += entity.physics.acceleration * deltaTick;
	entity.transform[3] += glm::vec4(entity.physics.velocity * deltaTick, 0);

	//RenderSystem::drawDebugArrow(entity.transform[3], entity.physics.velocity, glm::length(entity.physics.velocity) * 10, { 0, 1, 0 });
	//RenderSystem::drawDebugArrow(entity.transform[3], entity.physics.acceleration, glm::length(entity.physics.acceleration) * 100.0f);
}
