#include "Scene.h"

#include "Entity.h"

#include <algorithm>

Scene* Scene::s_currentScene = nullptr;

Entity& Scene::createEntity(size_t componentMask)
{
	Entity* newEntity;

	auto freeMem = std::find_if(m_entities.begin(), m_entities.end(), [](const std::unique_ptr<Entity>& entity) {
		return entity->hasComponents(COMPONENT_NONE);
	});
	if (freeMem != m_entities.end())
		// Reuse destroyed entity memory
		newEntity = freeMem->get();
	else {
		// Allocate memory for new entity
		m_entities.push_back(std::make_unique<Entity>());
		newEntity = m_entities.back().get();
		newEntity->network.id = -1;
	}

	// TODO: Add code to run the default constructors of each component
	// in the component mask so we don't have to remember to zero memory
	// and set special creation flags.

	newEntity->addComponents(componentMask);

	newEntity->network.isNewEntity = true;
	newEntity->network.priority = 0;

	return *newEntity;
}

void Scene::destroyEntity(Entity& entity)
{
	entity.destroy();
}

Entity& Scene::getEntity(size_t entityID)
{
	return *m_entities.at(entityID);
}

size_t Scene::getEntityCount()
{
	return m_entities.size();
}

void Scene::makeSceneCurrent(Scene* scene)
{
	s_currentScene = scene;
}

Scene * Scene::getCurrentScene()
{
	return s_currentScene;
}
