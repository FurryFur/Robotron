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
#include "Scene.h"
#include "Entity.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

#include <cmath>

Enemy01ControlSystem::Enemy01ControlSystem(Scene& scene)
	: m_scene{ scene }
{
}

//seek the enemy towards the player object every frame
void Enemy01ControlSystem::update(Entity& entity)
{
	if ((entity.componentMask & COMPONENT_ENEMY01) != COMPONENT_ENEMY01)
		return;
		
	//find the closest player and set the target to positoin to their current position.
	glm::vec3 targetPosition;

	for (unsigned int i = 0; i < m_scene.entities.size(); ++i)
	{
		if ((m_scene.entities.at(i).componentMask & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL)
		{
			targetPosition = m_scene.entities.at(i).transform[3];
		}
	}

	glm::vec3 pos = entity.transform[3];
	float moveSpeed = entity.controlVars.moveSpeed;

	//determine the desired veloctiy to reach the target
	float posMagnitude = glm::length(targetPosition);
	
	glm::vec3 desiredVelocity;
	if (posMagnitude < 5)
		desiredVelocity = (targetPosition - pos)/ posMagnitude * moveSpeed * (posMagnitude / 5); //apply arrival behaviour as the entity gets close to the target
	else
		desiredVelocity = (targetPosition - pos)/ posMagnitude * moveSpeed;

	//determine the steering vector from the current veloctiy to the desired velocity
	glm::vec3 steering = desiredVelocity - entity.physics.velocity;
	
	//further imrpovements:
	//truncate steering vector by a maximum amount
	//divide steering vector by object mass

	//determine the new velocty
	glm::vec3 newVelocity;
	newVelocity = entity.physics.velocity + steering;

	entity.transform[3] = { pos.x + newVelocity.x, pos.y + newVelocity.y, pos.z + newVelocity.z, 1}; //update the ojbects position with the new velocity

	return;
}