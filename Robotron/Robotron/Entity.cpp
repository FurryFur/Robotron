#include "Entity.h"

Entity::Entity()
	: m_componentMask{ 0 }
{
}

bool Entity::operator==(const Entity& rhs) const
{
	return this == &rhs;
}

bool Entity::hasComponents(size_t componentMask) const
{
	return (m_componentMask & componentMask) == componentMask;
}

bool Entity::hasComponentsAny(size_t componentMask) const
{
	return (m_componentMask & componentMask) > 0;
}

bool Entity::hasComponents() const
{
	return m_componentMask != 0;
}

void Entity::addComponents(size_t componentMask)
{
	if ((componentMask & COMPONENT_TRANSFORM) == COMPONENT_TRANSFORM) {
		m_componentMask |= COMPONENT_TRANSFORM;
		transform = {};
	}
	if ((componentMask & COMPONENT_PHYSICS) == COMPONENT_PHYSICS) {
		m_componentMask |= COMPONENT_PHYSICS;
		physics = {};
	}
	if ((componentMask & COMPONENT_MODEL) == COMPONENT_MODEL) {
		m_componentMask |= COMPONENT_MODEL;
		model = {};
	}
	if ((componentMask & COMPONENT_CAMERA) == COMPONENT_CAMERA) {
		m_componentMask |= COMPONENT_CAMERA;
	}
	if ((componentMask & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL) {
		m_componentMask |= COMPONENT_PLAYER_CONTROL;
		playerStats = {};
		controlVars = {};
	}
	if ((componentMask & COMPONENT_INPUT) == COMPONENT_INPUT) {
		m_componentMask |= COMPONENT_INPUT;
		input = {};
	}
	if ((componentMask & COMPONENT_INPUT_MAP) == COMPONENT_INPUT_MAP) {
		m_componentMask |= COMPONENT_INPUT_MAP;
		inputMap = {};
	}
	if ((componentMask & COMPONENT_LOGIC) == COMPONENT_LOGIC) {
		m_componentMask |= COMPONENT_LOGIC;
		logicVars = {};
	}
	if ((componentMask & COMPONENT_ZOMBIE) == COMPONENT_ZOMBIE) {
		m_componentMask |= COMPONENT_ZOMBIE;
		aiVariables = {};
		controlVars = {};
	}
	if ((componentMask & COMPONENT_SNAKE) == COMPONENT_SNAKE) {
		m_componentMask |= COMPONENT_SNAKE;
		aiVariables = {};
		controlVars = {};
	}
	if ((componentMask & COMPONENT_ENEMY_SHOOTER) == COMPONENT_ENEMY_SHOOTER) {
		m_componentMask |= COMPONENT_ENEMY_SHOOTER;
		aiVariables = {};
		controlVars = {};
	}
	if ((componentMask & COMPONENT_NETWORK) == COMPONENT_NETWORK) {
		m_componentMask |= COMPONENT_NETWORK;
		// network.id should be left alone as a non-negative value in combination
		// with an isNewEntity=true indicates to the network that an entity has
		// been destroyed and its memory reused.
		network.isNewEntity = true;
		network.priority = 999;
	}
	if ((componentMask & COMPONENT_SCOREPICKUP) == COMPONENT_SCOREPICKUP) {
		m_componentMask |= COMPONENT_SCOREPICKUP;
		aiVariables = {};
		controlVars = {};
	}
	if ((componentMask & COMPONENT_PLAYERBULLET) == COMPONENT_PLAYERBULLET) {
		m_componentMask |= COMPONENT_PLAYERBULLET;
		aiVariables = {};
		controlVars = {};
	}
	if ((componentMask & COMPONENT_ENEMYBULLET) == COMPONENT_ENEMYBULLET) {
		m_componentMask |= COMPONENT_ENEMYBULLET;
		aiVariables = {};
		controlVars = {};
	}
}

void Entity::destroy()
{
	m_componentMask = 0;
}
