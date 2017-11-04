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
#include "EntityUtils.h"
#include "GLUtils.h"
#include "GLMUtils.h"
#include "Scene.h"
#include "Entity.h"
#include "Clock.h"
#include "Utils.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <glm\gtx\euler_angles.hpp>

#include <cmath>

PlayerControlSystem::PlayerControlSystem(Scene& scene, Audio audio)
	: m_scene{ scene }
{
	m_audio = audio;
}

void PlayerControlSystem::update(Entity& entity, Clock& clock)
{
	// Filter movable
	const size_t kMovableMask = COMPONENT_PLAYER_CONTROL | COMPONENT_INPUT | COMPONENT_TRANSFORM;
	if (!entity.hasComponents(kMovableMask))
		return;

	float maxMoveSpeed = entity.controlVars.maxMoveSpeed;
	float maxAcceleration = entity.controlVars.maxAcceleration;
	float damping = entity.controlVars.damping;
	float orientationSensitivity = entity.controlVars.orientationSensitivity;

	//float deltaYaw = -orientationSensitivity * entity.input.orientationDelta.x;
	//float deltaPitch = -orientationSensitivity * entity.input.orientationDelta.y;
	//float deltaRoll = -orientationSensitivity * entity.input.orientationDelta.z;

	// Assume local space orientation for now
	// TODO: Add world space orientation
	//entity.transform.eulerAngles.x += deltaPitch;
	//entity.transform.eulerAngles.y += deltaYaw;
	//entity.transform.eulerAngles.z += deltaRoll;
	const float rollScale = 0.1f;
	const float pitchScale = 0.05f;
	entity.transform.eulerAngles.x = entity.physics.velocity.z * pitchScale;
	entity.transform.eulerAngles.z = -entity.physics.velocity.x * rollScale;

	glm::mat4 orientation = GLMUtils::eulerToMat(entity.transform.eulerAngles);
	glm::vec3 pos = entity.transform.position;

	// Displacement
	glm::vec3 axis = GLMUtils::limitVec(entity.input.axis, 1);
	if (!entity.controlVars.worldSpaceMove)
		axis = orientation * glm::vec4{ axis, 0 }; // Convert movement to local coordinates
	glm::vec3 acceleration = maxAcceleration * axis;
	if (axis != glm::vec3{ 0,0,0 }) {
		// TODO: Move this out of the player controller or place in physics component
		entity.aiVariables.previousVelocity = entity.physics.velocity;
		acceleration = acceleration - glm::length(acceleration) * entity.physics.velocity / maxMoveSpeed;
	} else
		acceleration = -damping * entity.physics.velocity;
	// Keep player in level
	if ((pos.x >= 20.0f || pos.x <= -20.0f || pos.z >= 20.0f || pos.z <= -20.0f))
		entity.physics.acceleration = -glm::vec3{ pos.x, 0, pos.z };
	else
		entity.physics.acceleration = acceleration;

	//Check if an enemy or enemy bullet is touching the player.
	//If so damage them and respawn them.
	// Find the closest player object to seek to.
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponentsAny(COMPONENT_ZOMBIE,
			COMPONENT_SNAKE, COMPONENT_ENEMY_SHOOTER, COMPONENT_ENEMYBULLET) // its an enemy bullet
			&& glm::length(m_scene.getEntity(i).transform.position - pos) < 1)		    // the player is within range to be damaged by it
		{
			entity.player.deathTime = clock.GetCurTime();
			entity.player.playerInfo->subtractLives(1);
			entity.player.isRespawning = true;
			entity.transform.position = glm::vec4{ 0.0f, 50.0f, 0.0f, 1.0f };

			m_audio.playSFX(PLAYER_DEAD);
		}
	}

	//Fire player bullets if they are set to fire
	// Check which way the player is firing and shoot in that direction.
	// Create a bullet on the player shooting right
	if ( (entity.input.shootRight || entity.input.shootLeft || entity.input.shootDown || entity.input.shootUp
	  ||  entity.input.shootRightUp || entity.input.shootRightDown || entity.input.shootLeftUp || entity.input.shootLeftDown)
	  && (entity.controlVars.lastFiringTime + entity.controlVars.firingSpeed <= clock.GetCurTime()))
	{
		entity.controlVars.lastFiringTime = clock.GetCurTime();

		glm::vec3 bulletVelocity;
		// The player is shooting right
		if (entity.input.shootRight)
			bulletVelocity = glm::vec3{ 10.0f, 0.0f, entity.physics.velocity.z };

		// The player is shooting left
		else if (entity.input.shootLeft)
			bulletVelocity = glm::vec3{ -10.0f, 0.0f, entity.physics.velocity.z };

		// The player is shooting down.
		else if (entity.input.shootDown)
			bulletVelocity = glm::vec3{ entity.physics.velocity.x, 0.0f, 10.0f };

		// The player is shooting up.
		else if (entity.input.shootUp)
			bulletVelocity = glm::vec3{ entity.physics.velocity.x, 0.0f, -10.0f };

		// The player is shooting right up
		else if (entity.input.shootRightUp)
			bulletVelocity = glm::vec3{ 7.07f , 0, -7.07f };

		// The player is shooting right down
		else if (entity.input.shootRightDown)
			bulletVelocity = glm::vec3{ 7.07f , 0, 7.07f };

		// The player is shooting left up
		else if (entity.input.shootLeftUp)
			bulletVelocity = glm::vec3{ -7.07f , 0, -7.07f };

		// The player is shooting left down
		else if (entity.input.shootLeftDown)
			bulletVelocity = glm::vec3{ -7.07f , 0, 7.07f };

		entity.input.shootRightDown = false;
		entity.input.shootRight = false;
		entity.input.shootRightUp = false;

		entity.input.shootLeft = false;
		entity.input.shootLeftUp = false;
		entity.input.shootLeftDown = false;

		entity.input.shootUp = false;
		entity.input.shootDown = false;

		// Create the bullet and apply the acceleration.
		TransformComponent transform{};
		transform.position = entity.transform.position;
		transform.scale = glm::vec3{ 0.5f, 0.5f, 0.5f };
		Entity& bullet = EntityUtils::createPlayerBullet(m_scene, transform);

		bullet.physics.velocity = bulletVelocity;
		bullet.playerBullet.playerIDNum = entity.player.playerInfo->getPlayerID();
		m_audio.playSFX(PLAYER_SHOOT);
	}
}