//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles movement ai
//				  for score pickups entity types. They
//				  follow a player who gets close to them do
//			      only stop following if they die.
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#define _USE_MATH_DEFINES

#include "ScorePickUpSystem.h"

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

ScorePickUpSystem::ScorePickUpSystem(Scene& scene, Audio audio)
	: m_scene{ scene }
{
	m_audio = audio;
}

//the ai movement behavious for enemy01s
void ScorePickUpSystem::update(Entity& entity, float deltaTick)
{
	// Check that the entity is an Enemy01 object before proceeding.
	if (!entity.hasComponents(COMPONENT_SCOREPICKUP))
		return;
	
	// If the player dies. Stop following them.
	if (entity.aiVariables.followEntity != NULL
     && entity.aiVariables.followEntity->player.isRespawning == true)
	{
		entity.aiVariables.followEntity = NULL;
	}


	// If Ai does not already have a player to follow, search for the nearest player within aggro range to follow
	if (entity.aiVariables.followEntity == NULL)
	{
		// Set the target position out of scope.
		glm::vec3 targetPosition = glm::vec3{ 100, 100, 100 };

		// Find the closest player object to follow.
		for (size_t i = 0; i < m_scene.getEntityCount(); ++i)
		{
			if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER_CONTROL)						                          //its a player object
				&& glm::length(m_scene.getEntity(i).transform.position - entity.transform.position) < glm::length(targetPosition - glm::vec3(entity.transform.position))     //it is the closet player to the target
				&& glm::length(m_scene.getEntity(i).transform.position - entity.transform.position) < 5)							                                      //the player is within the enemys aggro range
			{
				targetPosition = { m_scene.getEntity(i).transform.position.x, m_scene.getEntity(i).transform.position.y, m_scene.getEntity(i).transform.position.z };
				entity.aiVariables.followEntity = &m_scene.getEntity(i);

				// If following the controlling player, make a Sound
				if (entity.aiVariables.followEntity->hasComponents(COMPONENT_PLAYER_CONTROL))
					m_audio.playSFX(SCORE_PICKUP);
			}
		}
	}

	glm::vec3 acceleration;
	// Apply the seek and flock AI if target aquired.
	if (entity.aiVariables.followEntity != NULL)
	{
		entity.controlVars.maxMoveSpeed = 20.0f;
		// Add a pursue acceleration to the culmative acceleration.
		acceleration = followLeader(glm::vec3(entity.aiVariables.followEntity->transform.position), entity.aiVariables.followEntity->physics.velocity, entity.aiVariables.followEntity->aiVariables.previousVelocity, glm::vec3(entity.transform.position), entity.physics.velocity, entity.controlVars.maxMoveSpeed);

		std::vector<Entity*> nearbyNeighbours;
		// Find all the closest scorepickup neighbours and store them in a vector.
		for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
		{
			if (m_scene.getEntity(i).hasComponents(COMPONENT_SCOREPICKUP) &&
				(glm::length(glm::vec2(m_scene.getEntity(i).transform.position.x - entity.transform.position.x, m_scene.getEntity(i).transform.position.z - entity.transform.position.z))) <= 2.0f)
			{
				nearbyNeighbours.push_back(&m_scene.getEntity(i));
			}
		}

		// Add a flock acceleration to the culmative acceleration.
		if (nearbyNeighbours.size() > 0 && entity.aiVariables.followEntity->physics.velocity != glm::vec3{ 0,0,0 })
			acceleration += flock(nearbyNeighbours, glm::vec3(entity.transform.position), entity.physics.velocity, entity.controlVars.maxMoveSpeed);
	}
	// If no player to follow, just wander.
	else
	{
		entity.controlVars.maxMoveSpeed = 5.0f;
		acceleration = wander(entity);
	}

	steer(entity, acceleration);

	return;
}