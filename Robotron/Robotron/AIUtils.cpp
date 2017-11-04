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

#define _USE_MATH_DEFINES

#include "AIUtils.h"

#include "GLUtils.h"
#include "GLMUtils.h"
#include "Utils.h"
#include "Scene.h"
#include "RenderSystem.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

#include <cmath>

// Returns a force to move towards the target position.
glm::vec3 seek(glm::vec3 targetPosition, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed)
{

	// Determine the desired veloctiy to reach the target.
	glm::vec3 displacement = (targetPosition - currentPosition);
	float displacementMag = glm::length(displacement);
	glm::vec3 desiredVelocity;
	desiredVelocity = displacement / displacementMag * maxMoveSpeed;

	// Determine the steering vector from the current veloctiy to the desired velocity.
	glm::vec3 steering = desiredVelocity - currentVelocity;

	//RenderSystem::drawDebugArrow(currentPosition, desiredVelocity, glm::length(desiredVelocity) * 100, { 0, 0, 1 });

	return steering;
}

// Returns a force to move towards the target position, slows down with arrival when close.
glm::vec3 seekWithArrival(glm::vec3 targetPosition, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed)
{
	// Determine the desired veloctiy to reach the target.
	glm::vec3 displacement = (targetPosition - currentPosition);
	float displacementMag = glm::length(displacement);

	// Apply arrival behaviour as the entity gets close to the target.
	glm::vec3 desiredVelocity;
	if (displacementMag < 2)
		desiredVelocity = displacement / displacementMag * maxMoveSpeed * (displacementMag / 2);
	else
		desiredVelocity = displacement / displacementMag * maxMoveSpeed;

	// Determine the steering vector from the current veloctiy to the desired velocity.
	glm::vec3 steering = desiredVelocity - currentVelocity;

	//RenderSystem::drawDebugArrow(currentPosition, desiredVelocity, glm::length(desiredVelocity), { 1.0f, 1.0f, 0.0f });
	//RenderSystem::drawDebugArrow(currentPosition, steering, glm::length(steering), { 1.0f, 1.0f, 1.0f });

	return steering;
}

// Returns an acceleration to move to a move to pursue a target
glm::vec3 pursue(glm::vec3 targetPosition, glm::vec3 targetsVelocity, float targetsMoveSpeed, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed)
{
	float T = glm::length(glm::vec2{ targetPosition.x - currentPosition.x, targetPosition.z - currentPosition.z }) / targetsMoveSpeed;
	glm::vec3 futurePosition = targetPosition + targetsVelocity * T;
	
	futurePosition.y = currentPosition.y;

	return seek(futurePosition, currentPosition, currentVelocity, maxMoveSpeed);
}

// Returns an acceleration to move to a move to pursue a target
glm::vec3 evade(glm::vec3 targetPosition, glm::vec3 targetsVelocity, float targetsMoveSpeed, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed)
{
	float T = glm::length(glm::vec2{ targetPosition.x - currentPosition.x, targetPosition.z - currentPosition.z }) / targetsMoveSpeed;
	glm::vec3 futurePosition = targetPosition + targetsVelocity * (T/2.0f);

	futurePosition.y = currentPosition.y;

	return -seek(futurePosition, currentPosition, currentVelocity, maxMoveSpeed);
}

// Returns an acceleration to move to a random position forward from the current position.
glm::vec3 wander(Entity& entity)
{
	glm::vec3 currentPosition = entity.transform.position;
	glm::vec3 currentVelocity = entity.physics.velocity;
	float maxMoveSpeed = entity.controlVars.maxMoveSpeed;

	glm::vec3 targetPosition = entity.aiVariables.wanderPosition;

	// If stationary, move in a random direction.
	if (currentVelocity.x == 0 && currentVelocity.z == 0)
	{
		currentVelocity.x = randomReal<float>(-maxMoveSpeed, maxMoveSpeed);
		currentVelocity.z = sqrt(pow(maxMoveSpeed, 2) - pow(currentVelocity.x, 2));
		entity.physics.velocity = currentVelocity;
		targetPosition = glm::vec3{ (currentPosition.x + currentVelocity.x), currentPosition.y
			, (currentPosition.z + currentVelocity.z) };
	}
	// 1/50 chance to create a new position to wander to.
	if (randomInt(0, 50) == 1 || (glm::length(targetPosition - currentPosition) <= 2))
	{
		float radius = 4.0f;
		float a = randomReal<float>(0.0f, 1.0f);
		float b = randomReal<float>(0.0f, 1.0f);
		if (b < a)
		{
			float c = a;
			a = b;
			b = c;
		}

		float randomX = (b * radius * cos(2 * 3.14f * a / b));
		float randomZ = (b * radius * sin(2 * 3.14f * a / b));

		glm::vec3 circleCenter = (currentVelocity / glm::length(currentVelocity)) * maxMoveSpeed * 100.0f;

		// Wander to somewhere on a circle in front of the player
		targetPosition = { (currentPosition.x + circleCenter.x + randomX)
			, currentPosition.y
			,(currentPosition.z + circleCenter.z + randomZ) };

		entity.aiVariables.wanderPosition = targetPosition;
	}

	//RenderSystem::drawDebugArrow(currentPosition, targetPosition, { 0, 0, 1 });

	return seekWithArrival(targetPosition, currentPosition, currentVelocity, maxMoveSpeed);
}

// Compute the alignment section of flocking
glm::vec3 computeAlignment(std::vector<Entity*> nearbyNeighbours, glm::vec3 currentVelocity)
{
	int neighbourCount = 0;
	glm::vec3 cumulativeVelocity;

	// Find all the closest neighbours and add their velocity to the cumulative velocity.
	for (unsigned int i = 0; i < nearbyNeighbours.size(); ++i)
	{
		// Check its not looking at itself.
		if (currentVelocity != (*nearbyNeighbours.at(i)).physics.velocity)
		{
			++neighbourCount;
			cumulativeVelocity += (*nearbyNeighbours.at(i)).physics.velocity;
		}
	}

	// Return 0 if no neighbours nearby
	if (neighbourCount == 0 || cumulativeVelocity == glm::vec3{ 0,0,0 })
		return{ 0, 0, 0 };

	// Find the average of the cumlative velocity.
	glm::vec3 averageVelocity = { cumulativeVelocity.x / neighbourCount, cumulativeVelocity.y / neighbourCount, cumulativeVelocity.z / neighbourCount };

	// Return the normal of the average vector.
	return (averageVelocity / glm::length(averageVelocity));
}

// Compute the seperation cohesion of flocking
glm::vec3 computeCohesion(std::vector<Entity*> nearbyNeighbours, glm::vec3 currentPosition)
{
	int neighbourCount = 0;
	glm::vec3 cumulativePosition;

	// Find all the closest neighbours and add their position to a culmative position.
	for (unsigned int i = 0; i < nearbyNeighbours.size(); ++i)
	{
		glm::vec3 neighbourPosition = { (*nearbyNeighbours.at(i)).transform.position.x, (*nearbyNeighbours.at(i)).transform.position.y, (*nearbyNeighbours.at(i)).transform.position.z };
		// Check its not looking at itself.
		if (currentPosition != neighbourPosition)
		{
			++neighbourCount;
			cumulativePosition += neighbourPosition;
		}
	}

	// Return 0 if no neighbours nearby
	if (neighbourCount == 0 || cumulativePosition == glm::vec3{ 0,0,0 })
		return{ 0, 0, 0 };

	// Find the average of the cumlative position to get their center position.
	glm::vec3 averagePosition = { cumulativePosition.x / neighbourCount, cumulativePosition.y / neighbourCount, cumulativePosition.z / neighbourCount };

	//Find a vector from current position to the center of neighbours
	averagePosition = { averagePosition.x - currentPosition.x, averagePosition.y - currentPosition.y, averagePosition.z - currentPosition.z };

	// Return the normal of the vector.
	return (averagePosition / glm::length(averagePosition));
}

// Compute the separation section of flocking
glm::vec3 computeSeparation(std::vector<Entity*> nearbyNeighbours, glm::vec3 currentPosition)
{
	int neighbourCount = 0;
	glm::vec3 cumulativeDistance;

	// Find all the closest neighbours and add their distance away from the current object.
	for (unsigned int i = 0; i < nearbyNeighbours.size(); ++i)
	{
		glm::vec3 neighbourPosition = { (*nearbyNeighbours.at(i)).transform.position.x, (*nearbyNeighbours.at(i)).transform.position.y, (*nearbyNeighbours.at(i)).transform.position.z };
		// Check its not looking at itself.
		if (currentPosition != neighbourPosition)
		{
			++neighbourCount;
			cumulativeDistance += neighbourPosition - currentPosition;
		}
	}

	// Return 0 if no neighbours nearby
	if (neighbourCount == 0 || cumulativeDistance == glm::vec3{ 0,0,0 })
		return{ 0, 0, 0 };

	// Find the average of the cumlative distance.
	glm::vec3 averageVelocity = { cumulativeDistance.x / neighbourCount, cumulativeDistance.y / neighbourCount, cumulativeDistance.z / neighbourCount };

	// Sets it so the current object steers away form the average position of its neighbours.
	averageVelocity *= -1;

	// Return the normal of the average vector.
	return (averageVelocity / glm::length(averageVelocity));
}

//Returns an acceleration that allows the ojbect to flock with its nearest neighbours
glm::vec3 flock(std::vector<Entity*> nearbyNeighbours, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed)
{
	glm::vec3 alignment = computeAlignment(nearbyNeighbours, currentVelocity);
	glm::vec3 cohesion = computeCohesion(nearbyNeighbours, currentPosition);
	glm::vec3 separation = computeSeparation(nearbyNeighbours, currentPosition);

	// Determine the steering acceleration from the current veloctiy to the desired velocity.
	glm::vec3 flockVelocity = alignment + (cohesion * 0.5f) + separation;

	// Seek to the flock position only if that position is not current position.
	if (flockVelocity != glm::vec3{ 0,0,0 })
	{
		glm::vec3 targetPosition = currentPosition + flockVelocity;
		return seekWithArrival(targetPosition, currentPosition, currentVelocity, maxMoveSpeed);
	}
	else
		return{ 0,0,0 };
}

glm::vec3 followLeader(glm::vec3 targetPosition, glm::vec3 targetVelocity, glm::vec3 targetPreviousVelocity, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed)
{
	// Generate a vector in the opposite direction the target is heading.
	glm::vec2 tv;
	if (targetVelocity != glm::vec3{ 0,0,0 })
		tv = glm::vec2{ targetVelocity.x * -1.0f , targetVelocity.z * -1.0f };
	else
		tv = glm::vec2{ targetPreviousVelocity.x * -1.0f , targetPreviousVelocity.z * -1.0f };
	// Change its length to be equal to the desired follow distance.
	tv = (tv / glm::length(tv)) * 2.0f;
	// Find the position to follow.
	glm::vec3 followPosition = glm::vec3{ targetPosition.x + tv.x, targetPosition.y, targetPosition.z + tv.y };

	//Seek to that point
	return seekWithArrival(followPosition, currentPosition, currentVelocity, maxMoveSpeed);
}

void steer(Entity& entity, const glm::vec3& steeringAcceleration)
{
	// Weight the acceleration according to entities acceleration weight
	entity.physics.acceleration = steeringAcceleration * entity.controlVars.accelerationWeight;

	// Limit the steering acceleration.
	entity.physics.acceleration = GLMUtils::limitVec<glm::vec3>(entity.physics.acceleration, entity.controlVars.maxAcceleration);

	// Bounce of the boundraies of the walls
	glm::vec3 position = entity.transform.position;
	if (position.x > 20.0f || position.x < -20.0f)
	{
		entity.physics.velocity.x *= -1;
		entity.aiVariables.wanderPosition.x *= -1;
		if (position.x > 20.0f)
			entity.transform.position.x = 20.0f;
		else
			entity.transform.position.x = -20.0f;
	}

	if (position.z > 20.0f || position.z < -20.0f)
	{
		entity.physics.velocity.z *= -1;
		entity.aiVariables.wanderPosition.z *= -1;
		if (position.z > 20.0f)
			entity.transform.position.z = 20.0f;
		else
			entity.transform.position.z = -20.0f;
	}
}
