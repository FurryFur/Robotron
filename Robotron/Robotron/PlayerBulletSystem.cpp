//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles movement ai
//                for Enemy01 enemy types. They
//				  around the arena and pursue with
//				  flocking the closest player in their
//				  aggro range.
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#define _USE_MATH_DEFINES

#include "PlayerBulletSystem.h"

#include "GLUtils.h"
#include "GLMUtils.h"
#include "Utils.h"
#include "AIUtils.h"
#include "Scene.h"
//#include "Audio.h"
#include "Entity.h"
#include "RenderSystem.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

#include <cmath>

PlayerBulletSystem::PlayerBulletSystem(Scene& scene, Audio audio)
	: m_scene{ scene }
{
	m_audio = audio;
}

// Updates the position of the bullets every turn to travel in its travel directory.
// Checks to see if it has hit an enemy or a wall.
void PlayerBulletSystem::update(Entity& entity)
{
	// Check that the entity is an Playerbullet object before proceeding.
	if (!entity.hasComponents(COMPONENT_PLAYERBULLET))
		return;
	
	glm::vec3 pos = entity.transform.position;
	if (   pos.x > 20.0f
		|| pos.x < -20.0f
		|| pos.z > 20.0f
		|| pos.z < -20.0f)
	{
		m_scene.destroyEntity(entity);
		return;
	}

	// Cycle through all the entities in the scene and check if the bullet hit an enemy.
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponentsAny(COMPONENT_ZOMBIE, COMPONENT_SNAKE,
			   COMPONENT_ENEMY_SHOOTER, COMPONENT_ENEMYBULLET)
			&& glm::length(m_scene.getEntity(i).transform.position - pos) < 1)
		{
			// Destroy the bullet.
			m_scene.destroyEntity(entity);
			// Damage the enemy.
			--m_scene.getEntity(i).aiVariables.lives;
			// Check if the enemy is below 0 health before killing them and adding the score to the correct players score
			if (m_scene.getEntity(i).aiVariables.lives <= 0)
			{
				for (size_t j = 0; j < m_scene.getEntityCount(); ++j)
				{
					if (m_scene.getEntity(j).hasComponentsAny(COMPONENT_PLAYER_CONTROL) && entity.playerBullet.playerIDNum == m_scene.getEntity(j).player.playerIDNum)
					{
						m_scene.getEntity(j).player.playerInfo.score += m_scene.getEntity(i).aiVariables.score;
						m_scene.getEntity(j).player.extraLifeTrack += m_scene.getEntity(i).aiVariables.score;
					}
				}

				m_scene.destroyEntity(m_scene.getEntity(i));
				m_audio.playSFX(ENEMY_DEAD);
			}
			else
				m_audio.playSFX(ENEMY_HIT);
			return;
		}
	}

	return;
}