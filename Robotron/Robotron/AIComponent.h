//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A component for controlling AI logic
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#pragma once

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

class Entity;

struct AIComponent {
	// Used by score pickups to save which entity to follow using follow leader ai.
	Entity* followEntity;
	// Used to find the last direction the object was travelling before they were stationary.
	glm::vec3 previousVelocity;
	// Used to store the position to wander to.
	glm::vec3 wanderPosition;
	// Used for enemy02s to track the entities position in the snake 'train'.
	int positionInQueue;

	// Used by score pick ups to check whether it gives the player a life when recieved
	bool lifePickUp;

	// Used by score pickups and enemies to determine how many points they are worth
	int score;
	// Used by enemies to determine how many lives they have
	int lives;

};