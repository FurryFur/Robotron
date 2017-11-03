//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles movement ai
//                for Enemy01 enemy types. They
//				  around the arena and pursue with
//				  flocking the closest player in their
//				  aggro range.
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#define _USE_MATH_DEFINES

#include "Enemy01ControlSystem.h"

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

Enemy01ControlSystem::Enemy01ControlSystem(Scene& scene)
	: m_scene{ scene }
{

}

//the ai movement behavious for enemy01s
void Enemy01ControlSystem::update(Entity& entity, float deltaTick)
{
	// Check that the entity is an Enemy01 object before proceeding.
	if (!entity.hasComponents(COMPONENT_ZOMBIE))
		return;
	
	// Set the target position out of scope.
	glm::vec3 targetPosition  = glm::vec3{100, 100, 100};
	glm::vec3 targetsVelocity;
	float targetsMoveSpeed;
	glm::vec3 currentPosition = entity.transform.position;
	bool targetFound = false;

	// Find the closest player object to seek to.
	for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER_CONTROL)          						                               //its a player object
		 && glm::length(m_scene.getEntity(i).transform.position - currentPosition) < glm::length(targetPosition - currentPosition)     //it is the closet player to the target
		 && glm::length(m_scene.getEntity(i).transform.position - currentPosition) < 30)								                  //the player is within the enemys aggro range
		{
			targetPosition = { m_scene.getEntity(i).transform.position.x, currentPosition.y, m_scene.getEntity(i).transform.position.z};
			targetsVelocity = m_scene.getEntity(i).physics.velocity;
			targetsMoveSpeed = m_scene.getEntity(i).controlVars.maxMoveSpeed;
			targetFound = true;
		}
	}

	glm::vec3 acceleration;
	// Apply the seek and flock AI if target aquired.
	if (targetFound)
	{
		// Add a pursue acceleration to the culmative acceleration.
		acceleration = pursue(targetPosition, targetsVelocity, targetsMoveSpeed, currentPosition, entity.physics.velocity, entity.controlVars.maxMoveSpeed);

		std::vector<Entity*> nearbyNeighbours;
		// Find all the closest Enemy01 neighbours and store them in a vector.
		for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
		{
			if ((m_scene.getEntity(i).hasComponents(COMPONENT_ZOMBIE)) &&
			    (glm::length(glm::vec2(m_scene.getEntity(i).transform.position.x - currentPosition.x, m_scene.getEntity(i).transform.position.z - currentPosition.z))) <= 2.0f)
			{
				nearbyNeighbours.push_back(&m_scene.getEntity(i));
			}
		}

		// Add a flock acceleration to the culmative acceleration.
		if(nearbyNeighbours.size() > 0)
			acceleration += flock(nearbyNeighbours, currentPosition, entity.physics.velocity, entity.controlVars.maxMoveSpeed);
	}
	// If the seek returns 0 then wander.
	else
		acceleration = wander(entity);

	steer(entity, acceleration);

	return;
}