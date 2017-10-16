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
	// Order matters right now, selection buttons are assigned to the first four entities created
	SceneUtils::createSphere(scene, glm::translate({}, glm::vec3{ -1.5f, 1.5f, 0 }));
	SceneUtils::createQuad(scene,
		glm::translate({}, glm::vec3{ 1.5f, 1.5f, 0 })
		* glm::rotate(glm::mat4{}, static_cast<float>(-M_PI / 16), glm::vec3{ 1, 0, 0 }));
	SceneUtils::createCylinder(scene, 1.5, 1.5,
		glm::translate(glm::mat4{}, glm::vec3{ -1.5f, -1.5f, 0 })
		* glm::rotate(glm::mat4{}, static_cast<float>(M_PI / 4), glm::vec3{ 0, 0, 1 }));
	SceneUtils::createPyramid(scene, glm::translate({}, glm::vec3{ 1.5f, -1.5f, 0 }));

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
	size_t cameraEntity = SceneUtils::createCamera(scene, { 0, 0, 6 }, { 0, 0, 0 }, { 0, 1, 0 });
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
	m_networkSystem.beginFrame();

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
