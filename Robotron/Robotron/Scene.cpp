#include "Scene.h"

#include "Entity.h"
#include "SceneEventListener.h"

#include <algorithm>
#include <iostream>

Scene* Scene::s_currentScene = nullptr;

Entity& Scene::createEntity(size_t componentMask)
{
	Entity* newEntity;

	auto freeMem = std::find_if(m_entities.begin(), m_entities.end(), [](const std::unique_ptr<Entity>& entity) {
		return !entity->hasComponents();
	});
	if (freeMem != m_entities.end())
		// Reuse destroyed entity memory
		newEntity = freeMem->get();
	else {
		// Allocate memory for new entity
		newEntity = new Entity;
		m_entities.emplace_back(newEntity);
	}

	newEntity->addComponents(componentMask);

	triggerEntityCreationEvent(*newEntity);
	return *newEntity;
}

void Scene::destroyEntity(Entity& entity)
{
	triggerEntityDestructionEvent(entity);
	entity.m_componentMask = 0;
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

void Scene::registerEventListener(SceneEventListener* eventListener)
{
	if (eventListener) {
		m_eventListeners.push_back(eventListener);
	} else {
		// TODO: Add logging here
		std::cout << "WARNING: Tried to register a nullptr as a Scene Event Listener" << std::endl;
	}
}

void Scene::removeEventListener(SceneEventListener* eventListener)
{
	auto removeIt = std::remove(m_eventListeners.begin(), m_eventListeners.end(), eventListener);
	if (removeIt != m_eventListeners.end())
		m_eventListeners.erase(removeIt);
	else {
		// TODO: Add logging here
		std::cout << "WARNING: Tried to remove a Scene Event Listener that wasn't registered with the scene" << std::endl;
	}
}

void Scene::triggerEntityCreationEvent(Entity& entity)
{
	for (auto eventListener : m_eventListeners) {
		eventListener->onEntityCreation(entity);
	}
}

void Scene::triggerEntityDestructionEvent(Entity& entity)
{
	for (auto eventListener : m_eventListeners) {
		eventListener->onEntityDestruction(entity);
	}
}
