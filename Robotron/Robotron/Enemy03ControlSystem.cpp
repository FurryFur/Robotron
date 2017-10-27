//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles movement ai
//                for Enemy03 enemy types.
//				  aggro range.
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#define _USE_MATH_DEFINES

#include "Enemy03ControlSystem.h"

#include "GLUtils.h"
#include "GLMUtils.h"
#include "Utils.h"
#include "AIUtils.h"
#include "Scene.h"
#include "Entity.h"
#include "EntityUtils.h"
#include "RenderSystem.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

#include <cmath>

Enemy03ControlSystem::Enemy03ControlSystem(Scene& scene)
	: m_scene{ scene }
{

}

//the ai movement behavious for enemy03s
void Enemy03ControlSystem::update(Entity& entity, float deltaTick)
{
	// Check that the entity is an Enemy03 object before proceeding.
	if ((entity.componentMask & COMPONENT_ENEMY03) != COMPONENT_ENEMY03)
		return;
	
	// Set the target position out of scope.
	glm::vec3 targetPosition = glm::vec3{ 100, 100, 100 };
	glm::vec3 targetVelocity;
	glm::vec3 targetPreviousVelocity;
	float targetMoveSpeed;
	glm::vec3 currentPosition = glm::vec3{ entity.transform[3].x,  entity.transform[3].y,  entity.transform[3].z };
	bool targetFound = false;

	// Find the closest player object to seek to.
	for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if ((m_scene.getEntity(i).componentMask & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL						          //its a player object
			&& glm::length(m_scene.getEntity(i).transform[3] - entity.transform[3]) < glm::length(targetPosition - currentPosition)     //it is the closet player to the target
			&& glm::length(m_scene.getEntity(i).transform[3] - entity.transform[3]) < 40)								                  //the player is within the enemys aggro range
		{
			targetPosition = { m_scene.getEntity(i).transform[3].x, m_scene.getEntity(i).transform[3].y, m_scene.getEntity(i).transform[3].z };
			targetVelocity = m_scene.getEntity(i).physics.velocity;
			targetPreviousVelocity = m_scene.getEntity(i).aiVariables.previousVelocity;
			targetMoveSpeed = m_scene.getEntity(i).controlVars.moveSpeed;
			targetFound = true;
		}
	}

	glm::vec3 Acc;
	// Apply the seek and flock AI if target aquired.
	if (targetFound)
	{
		// Pursue to the target if above a certain distance away from them.
		if (glm::length(glm::vec2{ targetPosition.x, targetPosition.z } -glm::vec2{ entity.transform[3].x, entity.transform[3].z }) > 15)
			Acc = seekWithArrival(targetPosition, currentPosition, entity.physics.velocity, entity.controlVars.moveSpeed);
		// Avade from the target if too close to a player.
		else if (glm::length(glm::vec2{ targetPosition.x, targetPosition.z } -glm::vec2{ entity.transform[3].x, entity.transform[3].z }) < 5)
			Acc = evade(targetPosition, targetVelocity, targetMoveSpeed, currentPosition, entity.physics.velocity, entity.controlVars.moveSpeed);

		std::vector<Entity*> nearbyNeighbours;
		// Find all the closest Enemy01 neighbours and store them in a vector.
		for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
		{
			if (((m_scene.getEntity(i).componentMask & COMPONENT_ENEMY01) == COMPONENT_ENEMY01) &&
				(glm::length(glm::vec2(m_scene.getEntity(i).transform[3].x - entity.transform[3].x, m_scene.getEntity(i).transform[3].z - entity.transform[3].z))) <= 2.0f)
			{
				nearbyNeighbours.push_back(&m_scene.getEntity(i));
			}
		}

		// Add a flock acceleration to the culmative acceleration.
		if (nearbyNeighbours.size() > 0)
			Acc += flock(nearbyNeighbours, currentPosition, entity.physics.velocity, entity.controlVars.moveSpeed);

		// Occasionally shoot a bullet at the player
		// Create the bullet and apply the velocity.
		if (randomInt(0, 50) == 1)
		{
			Entity& bullet = EntityUtils::createEnemyBullet(m_scene,
				glm::translate({}, glm::vec3(entity.transform[3]))
				* glm::scale({}, glm::vec3{ 0.5f, 0.5f, 0.5f }));
			glm::vec3 bulletVelocity = ((targetPosition - glm::vec3(entity.transform[3]) + entity.physics.velocity) / (glm::length(targetPosition - glm::vec3(entity.transform[3])) + entity.physics.velocity)) * 0.2f;
			bullet.physics.velocity = bulletVelocity;
		}
	}
	// If the seek returns 0 then wander.
	else
		Acc = wander(entity);

	// Limit the steering acceleration.
	if (glm::length(Acc) > 0.1f)
		Acc = GLMUtils::limitVec<glm::vec3>(Acc, 0.1f);

	// Add the acceleration to the velocity.
	glm::vec3 newVelocity = glm::vec3{ entity.physics.velocity.x + Acc.x * deltaTick, 0, entity.physics.velocity.z + Acc.z * deltaTick };
	entity.physics.velocity += Acc * deltaTick;

	//TODO: ADD FLOCKING CODE HERE
	//if (isSeekingPlayer)
	//

	//const float kDebugScale = 100;
	//glm::vec3 position = glm::vec3(entity.transform[3]);
	//RenderSystem::drawDebugArrow(position, position + entity.physics.velocity * kDebugScale);
	//RenderSystem::drawDebugArrow(position + entity.physics.velocity * kDebugScale, Acc, glm::length(Acc) * kDebugScale);
	//RenderSystem::drawDebugArrow(m_scene, position, desiredVelocity, glm::length(desiredVelocity) * kDebugScale);

	glm::vec4 newPosition = entity.transform[3] + glm::vec4{ newVelocity, 0 };
	// Bounce of the boundraies of the walls
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