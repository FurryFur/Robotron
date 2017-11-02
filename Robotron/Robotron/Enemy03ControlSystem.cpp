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
#include "Clock.h"
#include "Entity.h"
#include "EntityUtils.h"
#include "RenderSystem.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

#include <cmath>

Enemy03ControlSystem::Enemy03ControlSystem(Scene& scene, Audio audio)
	: m_scene{ scene }
{
	m_audio = audio;
}

//the ai movement behavious for enemy03s
void Enemy03ControlSystem::update(Entity& entity, float deltaTick, Clock& clock)
{
	// Check that the entity is an Enemy03 object before proceeding.
	if (!entity.hasComponents(COMPONENT_ENEMY_SHOOTER))
		return;
	
	// Set the target position out of scope.
	glm::vec3 targetPosition = glm::vec3{ 100, 100, 100 };
	glm::vec3 targetsVelocity;
	glm::vec3 targetsPreviousVelocity;
	float targetsMoveSpeed;
	glm::vec3 currentPosition = glm::vec3{ entity.transform[3].x,  entity.transform[3].y,  entity.transform[3].z };
	bool targetFound = false;

	// Find the closest player object to seek to.
	for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER_CONTROL) 					                                           //its a player object
			&& glm::length(m_scene.getEntity(i).transform[3] - entity.transform[3]) < glm::length(targetPosition - currentPosition)     //it is the closet player to the target
			&& glm::length(m_scene.getEntity(i).transform[3] - entity.transform[3]) < 40)								                  //the player is within the enemys aggro range
		{
			targetPosition = { m_scene.getEntity(i).transform[3].x, entity.transform[3].y, m_scene.getEntity(i).transform[3].z };
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
		if (glm::length(glm::vec2{ targetPosition.x, targetPosition.z } -glm::vec2{ entity.transform[3].x, entity.transform[3].z }) > 15) {
			acceleration = seekWithArrival(targetPosition, currentPosition, entity.physics.velocity, entity.controlVars.maxMoveSpeed);
		}
		// Avade from the target if too close to a player.
		else if (glm::length(glm::vec2{ targetPosition.x, targetPosition.z } -glm::vec2{ entity.transform[3].x, entity.transform[3].z }) < 5)
			acceleration = evade(targetPosition, targetsVelocity, targetsMoveSpeed, currentPosition, entity.physics.velocity, entity.controlVars.maxMoveSpeed);

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


		// Shoot a bullet at the player
		// Check to see if the entity has not shot too recently
		if (entity.controlVars.lastFiringTime + entity.controlVars.firingSpeed <= clock.GetCurTime() && targetPosition.y == 1)
		{
			// Update the last entites last shot time
			entity.controlVars.lastFiringTime = clock.GetCurTime();
			entity.controlVars.firingSpeed = randomReal<double>(1.0f, 1.2f);
			
			Entity& bullet = EntityUtils::createEnemyBullet(m_scene,
				glm::translate({}, glm::vec3(entity.transform[3]))
				* glm::scale({}, glm::vec3{ 0.5f, 0.5f, 0.5f }));
			
			// Bullet shoot sound effect
			m_audio.playSFX(ENEMY_SHOOT);

			//Find a position to shoot at where the player will be
			float T = (glm::length(glm::vec2{ targetPosition.x - currentPosition.x, targetPosition.z - currentPosition.z }) / targetsMoveSpeed) / 2;
			glm::vec3 firingPosition = targetPosition + targetsVelocity * T;

			firingPosition.y = currentPosition.y;
			
			glm::vec3 bulletVelocity = ((firingPosition - glm::vec3(entity.transform[3])) / (glm::length(firingPosition - glm::vec3(entity.transform[3])))) * 10.0f;

			bulletVelocity.y = 0.0f;
			bullet.physics.velocity = bulletVelocity;
		}
	}
	// If the seek returns 0 then wander.
	else
		acceleration = wander(entity);

	steer(entity, acceleration);

	return;
}