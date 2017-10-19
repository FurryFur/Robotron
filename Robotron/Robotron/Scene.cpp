#include "Scene.h"

#include "Entity.h"

#include <algorithm>

Entity& Scene::createEntity(size_t componentMask)
{
	Entity* newEntity;

	auto freeMem = std::find_if(entities.begin(), entities.end(), [](const std::unique_ptr<Entity>& entity) {
		return entity->componentMask == COMPONENT_NONE;
	});
	if (freeMem != entities.end())
		// Reuse destroyed entity memory
		newEntity = freeMem->get();
	else {
		// Allocate memory for new entity
		entities.push_back(std::make_unique<Entity>());
		newEntity = entities.back().get();
	}

	newEntity->componentMask = componentMask;

	if (newEntity->hasAllComponents(COMPONENT_NETWORK))
		newEntity->network.isNewEntity = true;

	return *newEntity;
}

void Scene::destroyEntity(Entity& entity)
{
	entity.componentMask = COMPONENT_NONE;
}
