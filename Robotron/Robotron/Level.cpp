#include "Level.h"

Level::Level(GLFWwindow* window, Scene& scene):
	  m_renderSystem(window, scene)
	, m_movementSystem(scene)
	, m_inputSystem(window, scene)
	, m_gameplayLogicSystem(scene, m_inputSystem)
	, m_networkSystem()
	, m_scene(scene)
{
	m_window = window;

	// Create 3D entities.
	SceneUtils::createQuad(scene,
		glm::rotate(glm::mat4{}, static_cast<float>(M_PI / 2), glm::vec3{ 1, 0, 0 })
		* glm::scale({}, glm::vec3{ 20.0f, 20.0f, 1.0f }));

	SceneUtils::createPlayer(scene, 
		glm::translate({}, glm::vec3{ 0.0f, 1.0f, 0.0f }));

	// Create the skybox
	size_t skybox = SceneUtils::createSkybox(scene, {
		"Assets/Textures/Skybox/right.jpg",
		"Assets/Textures/Skybox/left.jpg",
		"Assets/Textures/Skybox/top.jpg",
		"Assets/Textures/Skybox/bottom.jpg",
		"Assets/Textures/Skybox/back.jpg",
		"Assets/Textures/Skybox/front.jpg",
	});
	// Set skybox as environment map for reflections
	m_renderSystem.setEnvironmentMap(skybox);

	// Setup the camera
	size_t cameraEntity = SceneUtils::createCamera(scene, { 0, 40, 20 }, { 0, 0, 0 }, { 0, 1, 0 });
	m_renderSystem.setCamera(cameraEntity);
}


Level::~Level()
{
}

void Level::process()
{
	// Do any operations that should only happen once per frame.
	m_inputSystem.beginFrame();
	m_renderSystem.beginRender();

	// Update all the entities using all the systems.
	for (size_t entityID = 0; entityID < SceneUtils::getEntityCount(m_scene); ++entityID) {
		m_gameplayLogicSystem.update(entityID);
		m_inputSystem.update(entityID);
		m_movementSystem.update(entityID);
		m_renderSystem.update(entityID);
	}

	// Do operations that should happen at the end of the frame.
	m_renderSystem.endRender();

	glfwPollEvents();
}
