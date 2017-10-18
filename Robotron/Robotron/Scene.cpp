#include "Scene.h"

#include "Entity.h"

#include <algorithm>

Entity& Scene::createEntity()
{
	// Reuse destroyed entityID memory
	auto freeMem = std::find_if(entities.begin(), entities.end(), [](const std::unique_ptr<Entity>& entity) {
		return entity->componentMask == COMPONENT_NONE;
	});
	if (freeMem != entities.end())
		return **freeMem;

	// Allocate memory for new entityID
	entities.push_back(std::make_unique<Entity>());

	return *entities.back();
}

void Scene::destroyEntity(Entity& entity)
{
	entity.componentMask = COMPONENT_NONE;
}
