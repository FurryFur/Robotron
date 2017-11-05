#pragma once

#include "Entity.h"

class SceneEventListener {
public:
	// Triggered when an entity is created in the scene.
	// This event will fire after the entity has been fully initialized.
	virtual void onEntityCreation(Entity& entity) = 0;

	// Triggered when an entity is destroyed in the scene.
	// The event will fire just before the entity is actually destroyed,
	// so the state of the entity will still be valid when this event
	// fires.
	virtual void onEntityDestruction(Entity& entity) = 0;
};