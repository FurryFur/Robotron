#pragma once

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
};