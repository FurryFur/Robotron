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

//the ai movement behavious for enemy01s
void Enemy01ControlSystem::update(Entity& entity)
{
	if ((entity.componentMask & COMPONENT_ENEMY01) != COMPONENT_ENEMY01)
		return;
		
	glm::vec4 targetPosition = { 100, 100, 100, 1 };
	glm::vec4 pos = entity.transform[3];
	float moveSpeed = entity.controlVars.moveSpeed;
	bool isSeekingPlayer = false;

	//find the closest player object to move to
	for (unsigned int i = 0; i < m_scene.entities.size(); ++i)
	{
		if ((m_scene.entities.at(i).componentMask & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL //its a player object
		  && glm::length(m_scene.entities.at(i).transform[3] - pos) < glm::length(targetPosition - pos)   //it is the closet player to the target
		  && glm::length(m_scene.entities.at(i).transform[3] - pos) < 15)								  //the player is within the enemys aggro range
		{
			targetPosition = m_scene.entities.at(i).transform[3];
			isSeekingPlayer = true;
		}
	}

	//TODO: ADD WANDERING CODE HERE
	//if (!isSeekingPlayer)
	//
	if (isSeekingPlayer == true)
	{
		//determine the desired veloctiy to reach the target
		float posMagnitude = glm::length(targetPosition - pos);

		glm::vec3 desiredVelocity;
		if (posMagnitude < 2)
			desiredVelocity = (targetPosition - pos) / posMagnitude * moveSpeed * (posMagnitude / 2); //apply arrival behaviour as the entity gets close to the target
		else
			desiredVelocity = (targetPosition - pos) / posMagnitude * moveSpeed;

		//determine the steering vector from the current veloctiy to the desired velocity
		glm::vec3 steering = desiredVelocity - entity.physics.velocity;

		//determine the new velocty
		glm::vec3 newVelocity;
		newVelocity = entity.physics.velocity + steering;

		//TODO: ADD FLOCKING CODE HERE
		//if (isSeekingPlayer)
		//

		entity.transform[3] = { pos.x + newVelocity.x, pos.y, pos.z + newVelocity.z, 1 }; //update the ojbects position with the new velocity
	}
	return;
}