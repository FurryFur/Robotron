#define _USE_MATH_DEFINES

#include "PhysicsSystem.h"

#include "RenderSystem.h"

#include <cmath>

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

	if (entity.hasComponents(COMPONENT_PLAYER)) {
		const float rollScale = 0.1f;
		const float pitchScale = 0.05f;
		entity.transform.eulerAngles.x = entity.physics.velocity.z * pitchScale;
		entity.transform.eulerAngles.z = -entity.physics.velocity.x * rollScale;
	} else {
		glm::vec3 velocity = entity.physics.velocity;
		entity.transform.eulerAngles.y = std::atan2f(-velocity.z, velocity.x) - static_cast<float>(M_PI_2);
	}

	//RenderSystem::drawDebugArrow(entity.lookAt[3], entity.physics.velocity, glm::length(entity.physics.velocity), { 0, 1, 0 });
	//RenderSystem::drawDebugArrow(glm::vec3(entity.lookAt[3]) + entity.physics.velocity, entity.physics.acceleration, glm::length(entity.physics.acceleration));
}
