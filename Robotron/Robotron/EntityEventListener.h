#pragma once

class Entity;

class EntityEventListener {
public:
	// Triggers just after the components in the component mask have been added
	virtual void onAddComponents(Entity& entity, size_t componentMask) = 0;
	// Triggers just before the components in the component mask are removed
	virtual void onRemoveComponents(Entity& entity, size_t componentMask) = 0;
};