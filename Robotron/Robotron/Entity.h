#pragma once

#include "InputComponent.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "PlayerControlComponent.h"
#include "LogicComponent.h"
#include "InputMapComponent.h"
#include "PhysicsComponent.h"

enum ComponentMask {
	COMPONENT_NONE = 0,
	COMPONENT_TRANSFORM = 1 << 0,
	COMPONENT_PHYSICS = 1 << 1,
	COMPONENT_MESH = 1 << 2,
	COMPONENT_MATERIAL = 1 << 3,
	COMPONENT_CAMERA = 1 << 4,
	COMPONENT_PLAYER_CONTROL = 1 << 5,
	COMPONENT_INPUT = 1 << 6,
	COMPONENT_INPUT_MAP = 1 << 7,
	COMPONENT_LOGIC = 1 << 8
};

struct Entity
{
	size_t componentMask;
	glm::mat4 transform;
	PhysicsComponent physics;
	MeshComponent mesh;
	MaterialComponent material;
	PlayerControlComponent controlVars;
	InputComponent input;
	InputMapComponent inputMap;
	LogicComponent logicVars;

	Entity() = default;
	Entity(Entity&&) = default;
	Entity(const Entity&) = delete;

	// Overload equality to check identity equivalence
	bool operator==(const Entity& rhs);
};

