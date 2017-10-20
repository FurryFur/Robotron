#pragma once

#include "InputComponent.h"
#include "ModelComponent.h"
#include "PlayerControlComponent.h"
#include "LogicComponent.h"
#include "InputMapComponent.h"
#include "PhysicsComponent.h"
#include "NetworkComponent.h"

enum ComponentMask {
	COMPONENT_NONE = 0,
	COMPONENT_TRANSFORM = 1 << 0,
	COMPONENT_PHYSICS = 1 << 1,
	COMPONENT_MODEL = 1 << 2,
	COMPONENT_CAMERA = 1 << 3,
	COMPONENT_PLAYER_CONTROL = 1 << 4,
	COMPONENT_INPUT = 1 << 5,
	COMPONENT_INPUT_MAP = 1 << 6,
	COMPONENT_LOGIC = 1 << 7,
	COMPONENT_ENEMY01 = 1 << 8,
	COMPONENT_NETWORK = 1 << 9
};

struct Entity
{
	size_t componentMask;
	glm::mat4 transform;
	PhysicsComponent physics;
	ModelComponent model;
	PlayerControlComponent controlVars;
	InputComponent input;
	InputMapComponent inputMap;
	LogicComponent logicVars;
	NetworkComponent network;

	Entity() = default;
	Entity(Entity&&) = default;
	Entity(const Entity&) = delete;

	// Overload equality to check identity equivalence
	bool operator==(const Entity& rhs) const;

	// Returns true if ALL the components in the supplied component 
	// mask are present in the entity.
	bool hasAllComponents(size_t componentMask) const;

	// Returns true if ANY of the components in the supplied component
	// mask are presnet in the entity.
	bool hasAnyComponent(size_t componentMaks) const;
};

