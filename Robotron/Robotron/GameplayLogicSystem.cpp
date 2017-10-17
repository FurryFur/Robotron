//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : The main game logic goes here.
//                This is class is intended to contain all non-portable code.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#include "GameplayLogicSystem.h"

#include "InputSystem.h"
#include "InputComponent.h"
#include "ShaderParams.h"
#include "Scene.h"
#include "Utils.h"
#include "LogicComponent.h"
#include "Entity.h"

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

GameplayLogicSystem::GameplayLogicSystem(Scene& scene, InputSystem& inputSystem)
	: m_scene{ scene }
	, m_oldPossessedEntity{ nullptr }
	, m_possessedEntity{ nullptr }
	, m_oldPossessedEntityUpdated{ true }
	, m_newPossessedEntityUpdated{ false }
	, m_dTheta{ 0.01f }
{
	inputSystem.registerKeyObserver(this);
}

void GameplayLogicSystem::update(Entity& entity)
{
	// Update the currently possessed entity
	if (!m_oldPossessedEntityUpdated && (&entity == m_oldPossessedEntity)) {
		// Update the old possessed entity so it doesn't respond to input
		entity.componentMask &= ~(COMPONENT_INPUT | COMPONENT_PLAYER_CONTROL);
		m_oldPossessedEntityUpdated = true;
		return;
	}
	if (!m_newPossessedEntityUpdated && (&entity == m_possessedEntity)) {
		// Update the new possessed entity so it response to input
		entity.componentMask |= (COMPONENT_INPUT | COMPONENT_PLAYER_CONTROL);
		m_newPossessedEntityUpdated = true;
		return;
	}

	const size_t kSpinnable = COMPONENT_TRANSFORM | COMPONENT_MESH | COMPONENT_LOGIC;
	if ((entity.componentMask & kSpinnable) == kSpinnable) {
		glm::vec3 pos = entity.transform[3];
		entity.transform[3] = { 0, 0, 0, 1 }; // Remove displacement temporarily
		entity.transform = glm::rotate(entity.transform, m_dTheta, entity.logicVars.rotationAxis);
		entity.transform[3] = glm::vec4{ pos, 1 }; // Add displacement back in
	}

	const size_t kModifiableGlossMask = COMPONENT_MATERIAL | COMPONENT_INPUT;
	if ((entity.componentMask & kModifiableGlossMask) == kModifiableGlossMask) {
		// Alter the glossiness of the possessed entity on key event
		ShaderParams& shaderParams = entity.material.shaderParams;
		if (entity.input.btn1Down) {
			shaderParams.glossiness = clamp(shaderParams.glossiness + 0.1f, 0.001f, 100.0f);
		}
		if (entity.input.btn2Down) {
			shaderParams.glossiness = clamp(shaderParams.glossiness - 0.1f, 0.001f, 100.0f);
		}
		if (entity.input.btn3Down) {
			shaderParams.metallicness = clamp(shaderParams.metallicness + 0.01f, 0.0f, 1.0f);
		}
		if (entity.input.btn4Down) {
			shaderParams.metallicness = clamp(shaderParams.metallicness - 0.01f, 0.0f, 1.0f);
		}
	}
}

void GameplayLogicSystem::keyCallback(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS)
		m_dTheta = -m_dTheta;

	// Skip keys we aren't interested in
	if (!(key == GLFW_KEY_1 || key == GLFW_KEY_2 || key == GLFW_KEY_3 
	   || key == GLFW_KEY_4)) // || key == GLFW_KEY_5))
		return;

	// Update the possessed entity state (which entity is currently being controled)
	m_oldPossessedEntity = m_possessedEntity;

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		m_possessedEntity = &m_scene.entities.at(0);
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		m_possessedEntity = &m_scene.entities.at(1);
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		m_possessedEntity = &m_scene.entities.at(2);
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		m_possessedEntity = &m_scene.entities.at(3);
	/*else if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		m_possessedEntity = 4;*/

	m_oldPossessedEntityUpdated = false;
	m_newPossessedEntityUpdated = false;
}
