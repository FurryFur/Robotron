#pragma once

#include "InputComponent.h"
#include "ModelComponent.h"
#include "ControlComponent.h"
#include "PlayerStatsComponent.h"
#include "PlayerBulletComponent.h"
#include "LogicComponent.h"
#include "InputMapComponent.h"
#include "PhysicsComponent.h"
#include "NetworkComponent.h"
#include "AIComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"

enum ComponentMask {
	COMPONENT_TRANSFORM = 1 << 0,
	COMPONENT_PHYSICS = 1 << 1,
	COMPONENT_MODEL = 1 << 2,
	COMPONENT_CAMERA = 1 << 3,
	COMPONENT_PLAYER_CONTROL = 1 << 4,
	COMPONENT_INPUT = 1 << 5,
	COMPONENT_INPUT_MAP = 1 << 6,
	COMPONENT_LOGIC = 1 << 7,
	COMPONENT_ZOMBIE = 1 << 8,
	COMPONENT_SNAKE = 1 << 9,
	COMPONENT_ENEMY_SHOOTER = 1 << 10,
	COMPONENT_NETWORK = 1 << 11,
	COMPONENT_SCOREPICKUP = 1 << 12,
	COMPONENT_PLAYERBULLET = 1 << 13,
	COMPONENT_ENEMYBULLET = 1 << 14,
	COMPONENT_PLAYER = 1 << 15,
};

class Entity {
	// The scene will handle all entity creation and destruction
	friend class Scene;

public:
	TransformComponent transform;
	PhysicsComponent physics;
	ModelComponent model;
	ControlComponent controlVars;
	PlayerStatsComponent player;
	PlayerBulletComponent playerBullet;
	InputComponent input;
	InputMapComponent inputMap;
	LogicComponent logicVars;
	NetworkComponent network;
	AIComponent aiVariables;
	CameraComponent camera;

	Entity(Entity&&) = default;
	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;
	Entity& operator=(Entity&&) = default;

	// Overload equality to check identity equivalence
	bool operator==(const Entity& rhs) const;

	// Returns true if ALL the specified components are present 
	// in the entity.
	template<typename ...ComponentTs>
	bool hasComponents(size_t first, ComponentTs... rest) const;

	// Returns true if ALL the components in the supplied component 
	// mask are present in the entity.
	bool hasComponents(size_t componentMask) const;

	// Returns true if ANY of the specified components are present
	// in the entity.
	template<typename ...ComponentTs>
	bool hasComponentsAny(size_t first, ComponentTs... rest) const;

	// Returns true if ANY of the components in the supplied component
	// mask are present in the entity.
	bool hasComponentsAny(size_t componentMask) const;

	// Returns true if this entity has any components
	bool hasComponents() const;

	// Adds multiple components to the entity
	template<typename ...ComponentTs>
	void addComponents(size_t first, ComponentTs... rest);

	// Adds a component to the entity.
	// A mask can also be specified to add more than one entity at
	// a time i.e. (COMPONENT_NETWORK | COMPONENT_TRANSFORM).
	void addComponents(size_t componentMask);

	// Removes a component from the entity.
	// A mask can also be specified to remove more than one entity at
	// a time i.e. (COMPONENT_NETWORK | COMPONENT_TRANSFORM).
	void removeComponents(size_t componentMask);

	// Removes multiple components from the entity
	template<typename ...ComponentTs>
	void removeComponents(size_t first, ComponentTs... rest);

private:
	Entity();

	// Returns true if ALL the specified components are present 
	// in the entity.
	template<typename ...ComponentTs>
	static bool matches(size_t componentMask, size_t first, ComponentTs... rest);

	// Returns true if ALL the components in the supplied component 
	// mask are present in the entity.
	static bool matches(size_t lhsComponentMask, size_t rhsComponentMask);

	// Returns true if ANY of the specified components are present
	// in the entity.
	template<typename ...ComponentTs>
	static bool matchesAny(size_t componentMask, size_t first, ComponentTs... rest);

	// Returns true if ANY of the components in the supplied component
	// mask are present in the entity.
	static bool matchesAny(size_t lhsComponentMask, size_t rhsComponentMask);

	size_t m_componentMask;
};

template<typename ...ComponentTs>
inline bool Entity::hasComponents(size_t first, ComponentTs... rest) const
{
	return hasComponents(first) && hasComponents(rest...);
}

template<typename ...ComponentTs>
inline bool Entity::hasComponentsAny(size_t first, ComponentTs ...rest) const
{
	return hasComponentsAny(first) || hasComponentsAny(rest...);
}

template<typename ...ComponentTs>
inline void Entity::addComponents(size_t first, ComponentTs... rest)
{
	addComponents(first);
	addComponents(rest...);
}

template<typename ...ComponentTs>
inline void Entity::removeComponents(size_t first, ComponentTs ...rest)
{
	removeComponents(first);
	removeComponents(rest...);
}

template<typename ...ComponentTs>
inline bool Entity::matches(size_t componentMask, size_t first, ComponentTs ...rest)
{
	return matches(componentMask, first) && matches(componentMask, rest...);
}

template<typename ...ComponentTs>
inline bool Entity::matchesAny(size_t componentMask, size_t first, ComponentTs ...rest)
{
	return matchesAny(componentMask, first) || matchesAny(componentMask, rest...);
}
