//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A header only collection of ai movement functions.
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#pragma once

#include "GLUtils.h"
#include "GLMUtils.h"
#include "Utils.h"
#include "Scene.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

#include <cmath>

//seek movement ai
glm::vec3 seek(Scene& scene, glm::vec4 currentPosition, glm::vec3 currentVelocity, float moveSpeed)
{
	glm::vec4 pos = currentPosition; //set the position to the entities current positoin
	glm::vec4 targetPosition = glm::vec4{ 100, 100, 100, 1 }; //set the target position out of scope
	bool isSeeking = false;

	//find the closest player object to move to
	for (unsigned int i = 0; i < scene.entities.size(); ++i)
	{
		if ((scene.entities.at(i).componentMask & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL //its a player object
			&& glm::length(scene.entities.at(i).transform[3] - pos) < glm::length(targetPosition - pos) //it is the closet player to the target
			&& glm::length(scene.entities.at(i).transform[3] - pos) < 15)								//the player is within the enemys aggro range
		{
			targetPosition = scene.entities.at(i).transform[3];
			isSeeking = true;
		}
	}

	if (!isSeeking)
		return glm::vec3{ 0,0,0 };
		
	//determine the desired veloctiy to reach the target
	float posMagnitude = glm::length(targetPosition - pos);

	glm::vec3 desiredVelocity;
	if (posMagnitude < 2)
		desiredVelocity = (targetPosition - pos) / posMagnitude * moveSpeed * (posMagnitude / 2); //apply arrival behaviour as the entity gets close to the target
	else
		desiredVelocity = (targetPosition - pos) / posMagnitude * moveSpeed;

	//determine the steering vector from the current veloctiy to the desired velocity
	glm::vec3 steering = desiredVelocity - currentVelocity;

	//determine the new velocty
	glm::vec3 newVelocity;
	newVelocity = currentVelocity + steering;

	return newVelocity;
}