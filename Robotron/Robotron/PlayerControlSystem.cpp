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

#include "PlayerControlSystem.h"

#include "ControlComponent.h"
#include "PlayerControlComponent.h"
#include "EntityUtils.h"
#include "GLUtils.h"
#include "GLMUtils.h"
#include "Scene.h"
#include "Entity.h"
#include "Clock.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

#include <cmath>

PlayerControlSystem::PlayerControlSystem(Scene& scene)
	: m_scene{ scene }
{
}

void PlayerControlSystem::update(Entity& entity, Clock& clock)
{
	if (!entity.hasComponents(COMPONENT_PLAYER_CONTROL))
		return;

	// Filter movable
	const size_t kMovableMask = COMPONENT_PLAYER_CONTROL | COMPONENT_INPUT | COMPONENT_TRANSFORM;
	const size_t kMovableCamMask = COMPONENT_CAMERA | COMPONENT_INPUT | COMPONENT_TRANSFORM;
	if (!entity.hasComponents(kMovableMask) && !entity.hasComponents(kMovableCamMask))
		return;

	float maxMoveSpeed = entity.controlVars.maxMoveSpeed;
	float orientationSensitivity = entity.controlVars.orientationSensitivity;

	float deltaAzimuth = -orientationSensitivity * entity.input.orientationDelta.x;
	float deltaElevation = -orientationSensitivity * entity.input.orientationDelta.y;
	float roll = -orientationSensitivity * entity.input.orientationDelta.z;
	glm::vec3 front = glm::vec3{ 0, 0, -1 };
	if (!entity.controlVars.worldSpaceMove)
		front = entity.transform * glm::vec4{ front, 0 }; // Convert movement to local coordinates
	glm::vec3 pos = entity.transform[3];
	glm::vec3 up = glm::vec3{ 0, 1, 0 };
	glm::vec3 right = glm::cross(front, up);

	// Displacement
	glm::vec3 axis = GLMUtils::limitVec(entity.input.axis, 1);
	if (!entity.controlVars.worldSpaceMove)
		axis = entity.transform * glm::vec4{ axis, 0 }; // Convert movement to local coordinates
	entity.physics.velocity = maxMoveSpeed * axis;
	if (axis != glm::vec3{ 0,0,0 })
		entity.aiVariables.previousVelocity = entity.physics.velocity;
	if (((pos + entity.physics.velocity).x < 20.0f) && ((pos + entity.physics.velocity).x > -20.0f))
		pos.x += entity.physics.velocity.x;
	if (((pos + entity.physics.velocity).z < 20.0f) && ((pos + entity.physics.velocity).z > -20.0f))
		pos.z += entity.physics.velocity.z;


	// Rotation
	// Prevent elevation going past 90 degrees
	glm::mat3 elevationMat{ 1 };
	float elevation = static_cast<float>(M_PI / 2 - std::acos(glm::dot(front, up)));
	if (std::abs(elevation + deltaElevation) < M_PI / 2)
		elevationMat = glm::rotate(deltaElevation, right);
	glm::mat3 azimuthMat = glm::rotate(deltaAzimuth, up);
	glm::mat3 rollMat = glm::rotate(roll, front);

	entity.transform[3] = { 0, 0, 0, 1 }; // Remove displacement temporarily
	entity.transform = glm::mat4{ rollMat * azimuthMat * elevationMat } *entity.transform; // Rotation without displacement
	entity.transform[3] = glm::vec4{ pos, 1 }; // Add displacement back in


	//Check if an enemy or enemy bullet is touching the player.
	//If so damage them and respawn them.
	// Find the closest player object to seek to.
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponentsAny(COMPONENT_ZOMBIE,
			COMPONENT_SNAKE, COMPONENT_ENEMY_SHOOTER, COMPONENT_ENEMYBULLET) // its an enemy bullet
			&& glm::length(m_scene.getEntity(i).transform[3] - entity.transform[3]) < 1)		    // the player is within range to be damaged by it
		{
			entity.playerStats.deathTime = clock.GetCurTime();
			--entity.playerStats.lives;
			entity.playerStats.isRespawning = true;
			entity.transform[3] = glm::vec4{ 0.0f, 50.0f, 0.0f, 1.0f };
		}
	}

	//Fire player bullets if they are set to fire
	// Check which way the player is firing and shoot in that direction.
	// Create a bullet on the player shooting right
	if (entity.controlVars.shootRight || entity.controlVars.shootLeft || entity.controlVars.shootDown || entity.controlVars.shootUp
	 || entity.controlVars.shootRightUp || entity.controlVars.shootRightDown || entity.controlVars.shootLeftUp || entity.controlVars.shootLeftDown)
	{
		glm::vec3 bulletVelocity;
		// The player is shooting right
		if (entity.controlVars.shootRight)
			bulletVelocity = glm::vec3{ 0.4f, 0.0f, entity.physics.velocity.z };

		// The player is shooting left
		else if (entity.controlVars.shootLeft)
			bulletVelocity = glm::vec3{ -0.4f, 0.0f, entity.physics.velocity.z };

		// The player is shooting down.
		else if (entity.controlVars.shootDown)
			bulletVelocity = glm::vec3{ entity.physics.velocity.x, 0.0f, 0.4f };

		// The player is shooting up.
		else if (entity.controlVars.shootUp)
			bulletVelocity = glm::vec3{ entity.physics.velocity.x, 0.0f, -0.4f };

		// The player is shooting right up
		else if (entity.controlVars.shootRightUp)
			bulletVelocity = glm::vec3{ 0.2828f , 0, -0.2828f };

		// The player is shooting right down
		else if (entity.controlVars.shootRightDown)
			bulletVelocity = glm::vec3{ 0.2828f , 0, 0.2828f };

		// The player is shooting left up
		else if (entity.controlVars.shootLeftUp)
			bulletVelocity = glm::vec3{ -0.2828f , 0, -0.2828f };

		// The player is shooting left down
		else if (entity.controlVars.shootLeftDown)
			bulletVelocity = glm::vec3{ -0.2828f , 0, 0.2828f };

		entity.controlVars.shootRightDown = false;
		entity.controlVars.shootRight = false;
		entity.controlVars.shootRightUp = false;

		entity.controlVars.shootLeft = false;
		entity.controlVars.shootLeftUp = false;
		entity.controlVars.shootLeftDown = false;

		entity.controlVars.shootUp = false;
		entity.controlVars.shootDown = false;

		// Create the bullet and apply the velocity.
		Entity& bullet = EntityUtils::createPlayerBullet(m_scene,
			glm::translate({}, glm::vec3(entity.transform[3]))
			* glm::scale({}, glm::vec3{ 0.5f, 0.5f, 0.5f }));

		bullet.physics.velocity = bulletVelocity;
	}
}