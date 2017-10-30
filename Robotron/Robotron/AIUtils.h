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
glm::vec3 seek(glm::vec3 targetPosition, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed);

// Returns a force to move towards the target position, slows down with arrival when close.
glm::vec3 seekWithArrival(glm::vec3 targetPosition, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed);

// Returns an acceleration to move to pursue a target
glm::vec3 pursue(glm::vec3 targetPosition, glm::vec3 targetVelocity, float tragetMoveSpeed, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed);

// Returns an acceleration to move to evade a target
glm::vec3 evade(glm::vec3 targetPosition, glm::vec3 targetVelocity, float tragetMoveSpeed, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed);

// Returns an acceleration to move to a random position forward from the current position.
glm::vec3 wander(Entity& entity);

// Compute the alignment section of flocking
glm::vec3 computeAlignment(std::vector<Entity*> nearbyNeighbours, glm::vec3 currentVelocity);

// Compute the seperation cohesion of flocking
glm::vec3 computeCohesion(std::vector<Entity*> nearbyNeighbours, glm::vec3 currentPosition);

// Compute the seperation section of flocking
glm::vec3 computeSeparation(std::vector<Entity*> nearbyNeighbours, glm::vec3 currentPosition);

//Returns an acceleration that allows the ojbect to flock with its nearest neighbours
glm::vec3 flock(std::vector<Entity*> nearbyNeighbours, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed);

glm::vec3 followLeader(glm::vec3 targetPosition, glm::vec3 targetVelocity, glm::vec3 targetPreviousVelocity, glm::vec3 currentPosition, glm::vec3 currentVelocity, float maxMoveSpeed);

// Applies steering to the entity while
// keeping the entity within the level boundaries.
// Entities will not exceed their maximum acceleration.
void steer(Entity&, const glm::vec3& steeringAcceleration);