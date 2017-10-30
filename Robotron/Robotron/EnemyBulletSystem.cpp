//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles behaviour
//				  for enemy bullet entity types. They
//				  travel in a straight line until they
//				  hit a play. they kill it and themselves. 
//				  or they
//				  hit a wall and get destroyed
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#define _USE_MATH_DEFINES

#include "EnemyBulletSystem.h"

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

EnemyBulletSystem::EnemyBulletSystem(Scene& scene)
	: m_scene{ scene }
{

}

// Updates the position of the bullets every turn to travel in its travel directory.
// Checks to see if it has hit a wall.
void EnemyBulletSystem::update(Entity& entity)
{
	// Check that the entity is an enemy bullet object before proceeding.
	if (!entity.hasComponents(COMPONENT_ENEMYBULLET))
		return;

	if ((entity.transform[3] + glm::vec4{ entity.physics.velocity, 0 }).x > 20.0f
		|| (entity.transform[3] + glm::vec4{ entity.physics.velocity, 0 }).x < -20.0f
		|| (entity.transform[3] + glm::vec4{ entity.physics.velocity, 0 }).z > 20.0f
		|| (entity.transform[3] + glm::vec4{ entity.physics.velocity, 0 }).z < -20.0f)
	{
		m_scene.destroyEntity(entity);
		return;
	}

	return;
}