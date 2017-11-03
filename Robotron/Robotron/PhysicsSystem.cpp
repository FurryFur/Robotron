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
	entity.transform.position += entity.physics.velocity * deltaTick;

	//RenderSystem::drawDebugArrow(entity.lookAt[3], entity.physics.velocity, glm::length(entity.physics.velocity), { 0, 1, 0 });
	//RenderSystem::drawDebugArrow(glm::vec3(entity.lookAt[3]) + entity.physics.velocity, entity.physics.acceleration, glm::length(entity.physics.acceleration));
}
