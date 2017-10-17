#include "Scene.h"

#include "Entity.h"

#include <algorithm>

Entity& Scene::createEntity()
{
	// Reuse destroyed entityID memory
	auto freeMem = std::find_if(entities.begin(), entities.end(), [](Entity& entity) {
		return entity.componentMask == COMPONENT_NONE;
	});
	if (freeMem != entities.end())
		return *freeMem;

	// Allocate memory for new entityID
	entities.emplace_back();

	return entities.back();
}

void Scene::destroyEntity(Entity& entity)
{
	entity.componentMask = COMPONENT_NONE;
}
