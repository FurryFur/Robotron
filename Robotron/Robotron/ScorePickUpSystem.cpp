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

ScorePickUpSystem::ScorePickUpSystem(Scene& scene)
	: m_scene{ scene }
{

}

//the ai movement behavious for enemy01s
void ScorePickUpSystem::update(Entity& entity, float deltaTick)
{
	// Check that the entity is an Enemy01 object before proceeding.
	if (!entity.hasComponents(COMPONENT_SCOREPICKUP))
		return;
	
	// If the player dies. Stop following them.
	if (entity.aiVariables.followEntity != NULL
     && entity.aiVariables.followEntity->playerStats.isRespawning == true)
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
				&& glm::length(m_scene.getEntity(i).transform[3] - entity.transform[3]) < glm::length(targetPosition - glm::vec3(entity.transform[3]))     //it is the closet player to the target
				&& glm::length(m_scene.getEntity(i).transform[3] - entity.transform[3]) < 5)							                                      //the player is within the enemys aggro range
			{
				targetPosition = { m_scene.getEntity(i).transform[3].x, m_scene.getEntity(i).transform[3].y, m_scene.getEntity(i).transform[3].z };
				entity.aiVariables.followEntity = &m_scene.getEntity(i);
			}
		}
	}

	glm::vec3 Acc;
	// Apply the seek and flock AI if target aquired.
	if (entity.aiVariables.followEntity != NULL)
	{
		// Add a pursue acceleration to the culmative acceleration.
		Acc = followLeader(glm::vec3(entity.aiVariables.followEntity->transform[3]), entity.aiVariables.followEntity->physics.velocity, entity.aiVariables.followEntity->aiVariables.previousVelocity, glm::vec3(entity.transform[3]), entity.physics.velocity, entity.controlVars.moveSpeed);

		std::vector<Entity*> nearbyNeighbours;
		// Find all the closest Enemy01 neighbours and store them in a vector.
		for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
		{
			if (m_scene.getEntity(i).hasComponents(COMPONENT_SCOREPICKUP) &&
				(glm::length(glm::vec2(m_scene.getEntity(i).transform[3].x - entity.transform[3].x, m_scene.getEntity(i).transform[3].z - entity.transform[3].z))) <= 2.0f)
			{
				nearbyNeighbours.push_back(&m_scene.getEntity(i));
			}
		}

		// Add a flock acceleration to the culmative acceleration.
		if (nearbyNeighbours.size() > 0 && entity.aiVariables.followEntity->physics.velocity != glm::vec3{ 0,0,0 })
			Acc += flock(nearbyNeighbours, glm::vec3(entity.transform[3]), entity.physics.velocity, entity.controlVars.moveSpeed);
	}
	// If no player to follow, just wander.
	else
		Acc = wander(entity);

	// Limit the steering acceleration.
	if (glm::length(Acc) > 3.2f)
		Acc = GLMUtils::limitVec<glm::vec3>(Acc, 3.2f);

	// Add the acceleration to the velocity.
	glm::vec3 newVelocity = glm::vec3{ entity.physics.velocity.x + Acc.x * deltaTick, entity.physics.velocity.y + Acc.y * deltaTick, entity.physics.velocity.z + Acc.z * deltaTick };
	entity.physics.velocity += Acc * deltaTick;

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