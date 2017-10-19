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
#include "RenderSystem.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

#include <cmath>

// Returns a force to move towards the target position.
glm::vec3 seek(glm::vec3 targetPosition, glm::vec3 currentPosition, glm::vec3 currentVelocity, float moveSpeed)
{

	// Determine the desired veloctiy to reach the target.
	glm::vec3 displacement = (targetPosition - currentPosition);
	float displacementMag = glm::length(displacement);
	glm::vec3 desiredVelocity;
	desiredVelocity = displacement / displacementMag * moveSpeed;

	// Determine the steering vector from the current veloctiy to the desired velocity.
	glm::vec3 steering = desiredVelocity - currentVelocity;

	RenderSystem::drawDebugArrow(currentPosition, desiredVelocity, glm::length(desiredVelocity) * 100);
	
	return steering;
}

// Returns a force to move towards the target position, slows down with arrival when close.
glm::vec3 seekWithArrival(glm::vec3 targetPosition, glm::vec3 currentPosition, glm::vec3 currentVelocity, float moveSpeed)
{
	
	// Determine the desired veloctiy to reach the target.
	glm::vec3 displacement = (targetPosition - currentPosition);
	float displacementMag = glm::length(displacement);

	// Apply arrival behaviour as the entity gets close to the target.
	glm::vec3 desiredVelocity;
	if (displacementMag < 2)
		desiredVelocity = displacement / displacementMag * moveSpeed * (displacementMag / 2);
	else
		desiredVelocity = displacement / displacementMag * moveSpeed;

	// Determine the steering vector from the current veloctiy to the desired velocity.
	glm::vec3 steering = desiredVelocity - currentVelocity;

	RenderSystem::drawDebugArrow(currentPosition, desiredVelocity, glm::length(desiredVelocity) * 100);

	return steering;
}

// Returns an acceleration to move to a move to pursue a target
glm::vec3 pursue(glm::vec3 targetPosition, glm::vec3 targetVelocity, float tragetMoveSpeed, glm::vec3 currentPosition, glm::vec3 currentVelocity, float moveSpeed)
{	
	float T = glm::length(glm::vec2{ targetPosition.x - currentPosition.x, targetPosition.z - currentPosition.z }) / tragetMoveSpeed;
	glm::vec3 futurePosition = targetPosition + targetVelocity * T;

	futurePosition.y = currentPosition.y;

	return seekWithArrival(futurePosition, currentPosition, currentVelocity, moveSpeed);
}

// Returns an acceleration to move to a random position forward from the current position.
glm::vec3 wander(glm::vec3 currentPosition, glm::vec3 currentVelocity, float moveSpeed)
{
	// Only change wandering with a 5% chance.
	if (randomInt(0, 50) != 1)
		return{ 0, 0, 0 };

	// Reduce the movement speed whilst wandering.
	moveSpeed = moveSpeed / 2;

	// If stationary, move in a random direction.
	if (currentVelocity.x == 0 && currentVelocity.z == 0)
	{
		currentVelocity.x = randomReal<float>(-moveSpeed, moveSpeed);
		currentVelocity.z = sqrt(pow(moveSpeed, 2) - pow(currentVelocity.x, 2));
	}

	float radius = 0.2f;
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

	glm::vec3 targetPosition = { (currentPosition.x + currentVelocity.x + randomX)
								, currentPosition.y
								,(currentPosition.z + currentVelocity.z + randomZ)};

	return seek(targetPosition, currentPosition, currentVelocity, moveSpeed);
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
		glm::vec3 neighbourPosition = { (*nearbyNeighbours.at(i)).transform[3].x, (*nearbyNeighbours.at(i)).transform[3].y, (*nearbyNeighbours.at(i)).transform[3].z };
		// Check its not looking at itself.
		if (currentPosition != neighbourPosition)
		{
			++neighbourCount;
			cumulativePosition += neighbourPosition;
		}
	}

	// Return 0 if no neighbours nearby
	if (neighbourCount == 0 || cumulativePosition == glm::vec3{0,0,0})
		return{ 0, 0, 0 };

	// Find the average of the cumlative position to get their center position.
	glm::vec3 averagePosition = { cumulativePosition.x / neighbourCount, cumulativePosition.y / neighbourCount, cumulativePosition.z / neighbourCount };

	//Find a vector from current position to the center of neighbours
	averagePosition = { averagePosition.x - currentPosition.x, averagePosition.y - currentPosition.y, averagePosition.z - currentPosition.z };

	// Return the normal of the vector.
	return (averagePosition / glm::length(averagePosition));
}

// Compute the seperation section of flocking
glm::vec3 computeSeparation(std::vector<Entity*> nearbyNeighbours, glm::vec3 currentPosition)
{
	int neighbourCount = 0;
	glm::vec3 cumulativeDistance;

	// Find all the closest neighbours and add their distance away from the current object.
	for (unsigned int i = 0; i < nearbyNeighbours.size(); ++i)
	{
		glm::vec3 neighbourPosition = { (*nearbyNeighbours.at(i)).transform[3].x, (*nearbyNeighbours.at(i)).transform[3].y, (*nearbyNeighbours.at(i)).transform[3].z };
		// Check its not looking at itself.
		if (currentPosition != neighbourPosition)
		{
			++neighbourCount;
			cumulativeDistance += neighbourPosition - currentPosition;
		}
	}
	
	// Return 0 if no neighbours nearby
	if (neighbourCount == 0 || cumulativeDistance == glm::vec3{0,0,0})
		return{ 0, 0, 0 };

	// Find the average of the cumlative distance.
	glm::vec3 averageVelocity = { cumulativeDistance.x / neighbourCount, cumulativeDistance.y / neighbourCount, cumulativeDistance.z / neighbourCount };

	// Sets it so the current object steers away form the average position of its neighbours.
	averageVelocity *= -1;

	// Return the normal of the average vector.
	return (averageVelocity / glm::length(averageVelocity));
}

//Returns an acceleration that allows the ojbect to flock with its nearest neighbours
glm::vec3 flock(std::vector<Entity*> nearbyNeighbours, glm::vec3 currentPosition, glm::vec3 currentVelocity, float moveSpeed)
{
	glm::vec3 alignment = computeAlignment(nearbyNeighbours, currentVelocity);
	glm::vec3 cohesion = computeCohesion(nearbyNeighbours, currentPosition);
	glm::vec3 separation = computeSeparation(nearbyNeighbours, currentPosition);

	// Determine the steering acceleration from the current veloctiy to the desired velocity.
	glm::vec3 flockVelocity = alignment + cohesion + glm::vec3{ separation.x * 2, separation.y * 2, separation.z * 2 };

	// Seek to the flock position only if that position is not current position.
	if (flockVelocity != glm::vec3{ 0,0,0 })
	{
		glm::vec3 targetPosition = currentPosition + flockVelocity;
		return seekWithArrival(targetPosition, currentPosition, currentVelocity, moveSpeed);
	}
	else
		return{ 0,0,0 };
}

glm::vec3 followLeader(glm::vec3 targetPosition, glm::vec3 targetVelocity, glm::vec3 targetPreviousVelocity, glm::vec3 currentPosition, glm::vec3 currentVelocity, float moveSpeed)
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
	return seekWithArrival(followPosition, currentPosition, currentVelocity, moveSpeed);
}