//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles movement based on an
//                entities input state.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
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

#include <cmath>

Enemy01ControlSystem::Enemy01ControlSystem(Scene& scene)
	: m_scene{ scene }
{

}

//the ai movement behavious for enemy01s
void Enemy01ControlSystem::update(Entity& entity)
{
	// Check that the entity is an Enemy01 object before proceeding.
	if ((entity.componentMask & COMPONENT_ENEMY01) != COMPONENT_ENEMY01)
		return;
	
	// Set the target position out of scope.
	glm::vec3 targetPosition  = glm::vec3{100, 100, 100};
	glm::vec3 targetVelocity;
	glm::vec3 targetPreviousVelocity;
	float targetMoveSpeed;
	glm::vec3 currentPosition = glm::vec3{ entity.transform[3].x,  entity.transform[3].y,  entity.transform[3].z};
	bool targetFound = false;

	// Find the closest player object to seek to.
	for (unsigned int i = 0; i < m_scene.entities.size(); ++i)
	{
		if ((m_scene.entities.at(i)->componentMask & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL						          //its a player object
			&& glm::length(m_scene.entities.at(i)->transform[3] - entity.transform[3]) < glm::length(targetPosition - currentPosition)     //it is the closet player to the target
			&& glm::length(m_scene.entities.at(i)->transform[3] - entity.transform[3]) < 15)								                  //the player is within the enemys aggro range
		{
			targetPosition = { m_scene.entities.at(i)->transform[3].x, m_scene.entities.at(i)->transform[3].y, m_scene.entities.at(i)->transform[3].z};
			targetVelocity = m_scene.entities.at(i)->physics.velocity;
			targetPreviousVelocity = m_scene.entities.at(i)->physics.previousVelocity;
			targetMoveSpeed = m_scene.entities.at(i)->controlVars.moveSpeed;
			targetFound = true;
		}
	}

	glm::vec3 Acc;
	// Apply the seek and flock AI if target aquired.
	if (targetFound)
	{
		// Add a pursue acceleration to the culmative acceleration.
		Acc = pursue(targetPosition, targetVelocity, targetMoveSpeed, currentPosition, entity.physics.velocity, entity.controlVars.moveSpeed);
		//Acc = followLeader(targetPosition, targetVelocity, targetPreviousVelocity, currentPosition, entity.physics.velocity, entity.controlVars.moveSpeed);

		std::vector<Entity*> nearbyNeighbours;
		// Find all the closest Enemy01 neighbours and store them in a vector.
		for (unsigned int i = 0; i < m_scene.entities.size(); ++i)
		{
			if (((m_scene.entities.at(i)->componentMask & COMPONENT_ENEMY01) == COMPONENT_ENEMY01) &&
				(glm::length(glm::vec2(m_scene.entities.at(i)->transform[3].x - entity.transform[3].x, m_scene.entities.at(i)->transform[3].z - entity.transform[3].z))) <= 2.0f)
			{
				nearbyNeighbours.push_back(m_scene.entities.at(i).get());
			}
		}

		// Add a flock acceleration to the culmative acceleration.
		if(nearbyNeighbours.size() > 0)
			Acc += flock(nearbyNeighbours, currentPosition, entity.physics.velocity, entity.controlVars.moveSpeed);
	}
	// If the seek returns 0 then wander.
	else
		Acc = wander(entity);

	// Limit the steering acceleration.
	if (glm::length(Acc) > 0.02f)
		Acc = GLMUtils::limitVec<glm::vec3>(Acc, 0.02f);

	// Add the acceleration to the velocity.
	glm::vec4 newVelocity = glm::vec4{ entity.physics.velocity.x + Acc.x, 0, entity.physics.velocity.z + Acc.z, 0 };
	entity.physics.velocity += Acc;

	//TODO: ADD FLOCKING CODE HERE
	//if (isSeekingPlayer)
	//

	const float kDebugScale = 100;
	glm::vec3 position = glm::vec3(entity.transform[3]);
	RenderSystem::drawDebugArrow(position, position + entity.physics.velocity * kDebugScale);
	RenderSystem::drawDebugArrow(position + entity.physics.velocity * kDebugScale, Acc, glm::length(Acc) * kDebugScale);
	//RenderSystem::drawDebugArrow(m_scene, position, desiredVelocity, glm::length(desiredVelocity) * kDebugScale);

	glm::vec4 newPosition = entity.transform[3] + newVelocity;
	if (newPosition.x > 20.0f || newPosition.x < -20.0f)
	{
		newVelocity.x *= -1;
		entity.physics.wanderPosition.x *= -1;
	}

	if (newPosition.z > 20.0f || newPosition.z < -20.0f)
	{
		newVelocity.z = -newVelocity.z;
		entity.physics.wanderPosition.z *= -1;
	}

	// Add the velocity to the position of the enemy.
	entity.transform[3] += newVelocity;

	return;
}