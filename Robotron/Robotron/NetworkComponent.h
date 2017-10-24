#pragma once

struct NetworkComponent {
	// A unique identifier for the entity on the network.
	// A value of -1 is used to indicate an unassigned network id.
	// A unique id will then be assigned by the server.
	std::int32_t id;

	// Indicates that the associated entity has been newly created.
	// A server should respond by notifying clients to create
	// new ghost objects.
	bool isNewEntity;
};