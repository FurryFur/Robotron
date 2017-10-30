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
	if (!entity.hasComponents(COMPONENT_ENEMY_SHOOTER))
		return;
	
	// Set the target position out of scope.
	glm::vec3 targetsPosition = glm::vec3{ 100, 100, 100 };
	glm::vec3 targetsVelocity;
	glm::vec3 targetsPreviousVelocity;
	float targetsMoveSpeed;
	glm::vec3 currentPosition = glm::vec3{ entity.transform[3].x,  entity.transform[3].y,  entity.transform[3].z };
	bool targetFound = false;

	// Find the closest player object to seek to.
	for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER_CONTROL) 					                                           //its a player object
			&& glm::length(m_scene.getEntity(i).transform[3] - entity.transform[3]) < glm::length(targetsPosition - currentPosition)     //it is the closet player to the target
			&& glm::length(m_scene.getEntity(i).transform[3] - entity.transform[3]) < 40)								                  //the player is within the enemys aggro range
		{
			targetsPosition = { m_scene.getEntity(i).transform[3].x, m_scene.getEntity(i).transform[3].y, m_scene.getEntity(i).transform[3].z };
			targetsVelocity = m_scene.getEntity(i).physics.velocity;
			targetsPreviousVelocity = m_scene.getEntity(i).aiVariables.previousVelocity;
			targetsMoveSpeed = m_scene.getEntity(i).controlVars.maxMoveSpeed;
			targetFound = true;
		}
	}

	glm::vec3 acceleration;
	// Apply the seek and flock AI if target aquired.
	if (targetFound)
	{
		// Pursue to the target if above a certain distance away from them.
		if (glm::length(glm::vec2{ targetsPosition.x, targetsPosition.z } -glm::vec2{ entity.transform[3].x, entity.transform[3].z }) > 15)
			acceleration = seekWithArrival(targetsPosition, currentPosition, entity.physics.velocity, entity.controlVars.maxMoveSpeed);
		// Avade from the target if too close to a player.
		else if (glm::length(glm::vec2{ targetsPosition.x, targetsPosition.z } -glm::vec2{ entity.transform[3].x, entity.transform[3].z }) < 5)
			acceleration = evade(targetsPosition, targetsVelocity, targetsMoveSpeed, currentPosition, entity.physics.velocity, entity.controlVars.maxMoveSpeed);

		std::vector<Entity*> nearbyNeighbours;
		// Find all the closest Enemy01 neighbours and store them in a vector.
		for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
		{
			if ((m_scene.getEntity(i).hasComponents(COMPONENT_ZOMBIE)) &&
			    (glm::length(glm::vec2(m_scene.getEntity(i).transform[3].x - entity.transform[3].x, m_scene.getEntity(i).transform[3].z - entity.transform[3].z))) <= 2.0f)
			{
				nearbyNeighbours.push_back(&m_scene.getEntity(i));
			}
		}

		// Add a flock acceleration to the culmative acceleration.
		if (nearbyNeighbours.size() > 0)
			acceleration += flock(nearbyNeighbours, currentPosition, entity.physics.velocity, entity.controlVars.maxMoveSpeed);

		// Occasionally shoot a bullet at the player
		// Create the bullet and apply the velocity.
		if (randomInt(0, 50) == 1)
		{
			Entity& bullet = EntityUtils::createEnemyBullet(m_scene,
				glm::translate({}, glm::vec3(entity.transform[3]))
				* glm::scale({}, glm::vec3{ 0.5f, 0.5f, 0.5f }));
			glm::vec3 bulletVelocity = ((targetsPosition - glm::vec3(entity.transform[3]) + entity.physics.velocity) / (glm::length(targetsPosition - glm::vec3(entity.transform[3])) + entity.physics.velocity)) * 10.0f;
			bullet.physics.velocity = bulletVelocity;
		}
	}
	// If the seek returns 0 then wander.
	else
		acceleration = wander(entity);

	steer(entity, acceleration);

	return;
}