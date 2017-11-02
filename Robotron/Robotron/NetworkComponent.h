#pragma once

#include "InputComponent.h"

#include <glm\glm.hpp>

#include <cstdint>

struct NetworkComponent {
	NetworkComponent();

	// A unique identifier for the entity on the network.
	// A value of -1 is used to indicate an unassigned network id.
	// A unique id will then be assigned by the server.
	std::int32_t id;

	// Indicates that the associated entity has been newly created.
	// A server should respond by notifying clients to create
	// new ghost objects.
	bool isNewEntity;

	// The priority that this entity has when the server is decided
	// which entities should have snapshots sent to clients.
	// This priority will slowly accumulate the longer an entity
	// goes without having a snapshot sent out.
	int priority;

	// This variable is used by the server to fast forward inputs
	// to the lastest version received, if no inputs are received 
	// within a frame, and all previous inputs have already been processed.
	InputComponent lastInputReceived;

	// This variable is used, and updated, by the server to check 
	// whether an input was received this frame or not. The server
	// can then decide whether to fast forward the entities current
	// input state to the last version it has seen.
	bool receivedInputThisFrame;
};