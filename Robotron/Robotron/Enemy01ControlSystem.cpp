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

#include "Enemy01ControlSystem.h"

#include "GLUtils.h"
#include "GLMUtils.h"
#include "Utils.h"
#include "AIUtils.h"
#include "Scene.h"
#include "Entity.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

#include <cmath>

Enemy01ControlSystem::Enemy01ControlSystem(Scene& scene)
	: m_scene{ scene }
{
}

//the ai movement behavious for enemy01s
void Enemy01ControlSystem::update(Entity& entity)
{
	if ((entity.componentMask & COMPONENT_ENEMY01) != COMPONENT_ENEMY01)
		return;
	
	entity.physics.velocity = seek(m_scene, entity.transform[3], entity.physics.velocity, entity.controlVars.moveSpeed);

	glm::vec4 newVelocity = glm::vec4{ entity.physics.velocity.x, entity.physics.velocity.y, entity.physics.velocity.z, 0 };
	entity.transform[3] += newVelocity;
	
	return;
}