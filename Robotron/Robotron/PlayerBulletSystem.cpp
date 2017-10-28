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
#include "Entity.h"
#include "RenderSystem.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

#include <cmath>

PlayerBulletSystem::PlayerBulletSystem(Scene& scene)
	: m_scene{ scene }
{

}

// Updates the position of the bullets every turn to travel in its travel directory.
// Checks to see if it has hit an enemy or a wall.
void PlayerBulletSystem::update(Entity& entity, float deltaTick)
{
	// Check that the entity is an Enemy02 object before proceeding.
	if (!entity.hasComponents(COMPONENT_PLAYERBULLET))
		return;
	
	if ((   entity.transform[3] + glm::vec4{ entity.physics.velocity, 0 }).x > 20.0f
		|| (entity.transform[3] + glm::vec4{ entity.physics.velocity, 0 }).x < -20.0f
		|| (entity.transform[3] + glm::vec4{ entity.physics.velocity, 0 }).z > 20.0f
		|| (entity.transform[3] + glm::vec4{ entity.physics.velocity, 0 }).z < -20.0f)
	{
		m_scene.destroyEntity(entity);
		return;
	}

	// Cycle through all the entities in the scene and check if the bullet hit an enemy.
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponentsAny(COMPONENT_ZOMBIE, COMPONENT_SNAKE,
			   COMPONENT_ENEMY_SHOOTER)
			&& glm::length(m_scene.getEntity(i).transform[3] - entity.transform[3]) < 1)
		{
			// Destroy the bullet.
			m_scene.destroyEntity(entity);
			// Damage the enemy.
			--m_scene.getEntity(i).aiVariables.lives;
			// Check if the enemy is below 0 health before killing them.
			if(m_scene.getEntity(i).aiVariables.lives <= 0)
				m_scene.destroyEntity(m_scene.getEntity(i));
			return;
		}
	}

	entity.transform[3] += glm::vec4{ entity.physics.velocity, 0 };

	return;
}