//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles movement ai
//                for Enemy02enemy types.
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#define _USE_MATH_DEFINES

#include "Enemy02ControlSystem.h"

#include "GLUtils.h"
#include "GLMUtils.h"
#include "Utils.h"
#include "AIUtils.h"
#include "Scene.h"
#include "Entity.h"
#include "RenderSystem.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

#include <cmath>

Enemy02ControlSystem::Enemy02ControlSystem(Scene& scene)
	: m_scene{ scene }
{

}

//the ai movement behavious for enemy01s
void Enemy02ControlSystem::update(Entity& entity, float deltaTick)
{
	// Check that the entity is an Enemy02 object before proceeding.
	if ((entity.componentMask & COMPONENT_ENEMY02) != COMPONENT_ENEMY02)
		return;

	int followNumber = entity.aiVariables.positionInQueue;
	glm::vec3 followPosition;

	// Cycle through the other entities in the scene and find another enemy02 with the next lowest position in queue
	for (unsigned int i = 0; i < m_scene.entities.size(); ++i)
	{
		if ((m_scene.entities.at(i)->componentMask & COMPONENT_ENEMY02) == COMPONENT_ENEMY02 &&
			m_scene.entities.at(i)->aiVariables.positionInQueue ==  followNumber - 1)
		{
			followPosition = m_scene.entities.at(i)->transform[3];
			followNumber = m_scene.entities.at(i)->aiVariables.positionInQueue;
			break;
		}
	}

	glm::vec3 Acc;
	//Wander around the map if the first in the queue
	if (followNumber == entity.aiVariables.positionInQueue)
		Acc = wander(entity);
	else {
		Acc = seekWithArrival(followPosition, glm::vec3{ entity.transform[3] }, entity.physics.velocity, entity.controlVars.moveSpeed);
	}

	// Limit the steering acceleration.
	if (glm::length(Acc) > 0.002f)
		Acc = GLMUtils::limitVec<glm::vec3>(Acc, 0.002f);

	// Add the acceleration to the velocity.
	glm::vec3 newVelocity = glm::vec3{ entity.physics.velocity.x + Acc.x , 0, entity.physics.velocity.z + Acc.z};
	entity.physics.velocity += Acc;

	//TODO: ADD FLOCKING CODE HERE
	//if (isSeekingPlayer)
	//

	//const float kDebugScale = 100;
	//glm::vec3 position = glm::vec3(entity.transform[3]);
	//RenderSystem::drawDebugArrow(position, position + entity.physics.velocity * kDebugScale);
	//RenderSystem::drawDebugArrow(position + entity.physics.velocity * kDebugScale, Acc, glm::length(Acc) * kDebugScale);
	//RenderSystem::drawDebugArrow(m_scene, position, desiredVelocity, glm::length(desiredVelocity) * kDebugScale);

	glm::vec4 newPosition = entity.transform[3] + glm::vec4{ newVelocity, 0 };
	if (newPosition.x > 20.0f || newPosition.x < -20.0f)
	{
		newVelocity.x *= -1;
		entity.aiVariables.wanderPosition.x *= -1;
		if (newPosition.x > 20.0f)
			entity.transform[3].x = 20.0f;
		else
			entity.transform[3].x = -20.0f;
	}

	if (newPosition.z > 20.0f || newPosition.z < -20.0f)
	{
		newVelocity.z *= -1;
		entity.aiVariables.wanderPosition.z *= -1;
		if (newPosition.z > 20.0f)
			entity.transform[3].z = 20.0f;
		else
			entity.transform[3].z = -20.0f;
	}

	// Add the velocity to the position of the enemy.
	entity.physics.velocity = newVelocity;
	entity.transform[3] += glm::vec4{ entity.physics.velocity, 0 };

	return;
}