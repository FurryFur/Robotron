//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles movement ai
//                for Enemy02enemy types.
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#define _USE_MATH_DEFINES

#include "Enemy02ControlSystem.h"

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

Enemy02ControlSystem::Enemy02ControlSystem(Scene& scene)
	: m_scene{ scene }
{

}

//the ai movement behavious for enemy01s
void Enemy02ControlSystem::update(Entity& entity, float deltaTick)
{
	// Check that the entity is an Enemy02 object before proceeding.
	if (!entity.hasComponents(COMPONENT_SNAKE))
		return;

	int followNumber = entity.aiVariables.positionInQueue;
	glm::vec3 followPosition;
	Entity* followEntity = nullptr;

	// Cycle through the other entities in the scene and find another enemy02 with the next lowest position in queue
	for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponents(COMPONENT_SNAKE) 
		 && m_scene.getEntity(i).aiVariables.positionInQueue ==  followNumber - 1)
		{
			followPosition = m_scene.getEntity(i).transform[3];
			followPosition.y = entity.transform[3].y; // Keep enemy on same plane
			followNumber = m_scene.getEntity(i).aiVariables.positionInQueue;
			followEntity = &m_scene.getEntity(i);
			break;
		}
	}

	glm::vec3 acceleration;
	//Wander around the map if the first in the queue
	if (followNumber == entity.aiVariables.positionInQueue)
	{
		acceleration = wander(entity);

		float tailLength = 1;

		for (unsigned int i = 1; i <= 10; ++i)
		{
			bool tailBreak = true;
			// Cycle through the other entities in the scene and find another enemy02 with the next lowest position in queue
			for (unsigned int j = 0; j < m_scene.getEntityCount(); ++j)
			{
				if (m_scene.getEntity(j).hasComponents(COMPONENT_SNAKE)
					&& m_scene.getEntity(j).aiVariables.positionInQueue == entity.aiVariables.positionInQueue + i)
				{
					++tailLength;
					tailBreak = false;
				}
			}
			if (tailBreak)
				break;
		}

		entity.controlVars.maxMoveSpeed = 30 / (tailLength / 2.666f);
	}
	else {
		acceleration = seekWithArrival(followPosition, glm::vec3{ entity.transform[3] }, entity.physics.velocity, entity.controlVars.maxMoveSpeed);

		if(followEntity != nullptr)
			entity.controlVars.maxMoveSpeed = followEntity->controlVars.maxMoveSpeed;
	}

	steer(entity, acceleration);

	return;
}