#pragma once

struct NetworkComponent {
	// A unique identifier for the entity on the network.
	// A value of -1 is used to indicate a new network entity.
	// A unique id will then be assigned by the server and clients
	// will be notified of entity creation.
	size_t id;
	bool isNewEntity;
};