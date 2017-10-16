#include "Level.h"

#include "NetworkClientSystem.h"

#include <iostream>
#include <string>

Level::Level(GLFWwindow* window, Scene& scene, int levelNum):
	  m_renderSystem(window, scene)
	, m_movementSystem(scene)
	, m_inputSystem(window, scene)
	, m_scene(scene)
{
	m_window = window;
	m_levelNum = levelNum;

	std::string strServerMode;
	std::cout << "Run in server mode: ";
	std::cin >> strServerMode;
	if (strServerMode == "y")
		m_networkSystem = std::make_unique<NetworkServerSystem>(scene);
	else
		m_networkSystem = std::make_unique<NetworkClientSystem>(scene);

	// Create 3D entities.
	SceneUtils::createQuad(scene,
		glm::rotate(glm::mat4{}, static_cast<float>(M_PI / 2), glm::vec3{ 1, 0, 0 })
		* glm::scale({}, glm::vec3{ 20.0f, 20.0f, 1.0f }));

	SceneUtils::createPlayer(scene, 
		glm::translate({}, glm::vec3{ 0.0f, 1.0f, 0.0f }));

	//create the number of enemy01 based on current level with some random variance.
	unsigned int numberOfEnemy01 = 9 + m_levelNum + randomInt(-2, 2);
	
	for (unsigned int i = 0; i <= numberOfEnemy01; ++i)
	{
		float randX = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
		{
			randX += 25;
		}
		float randZ = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
		{
			randZ += 25;
		}
		SceneUtils::createEnemy01(scene,
			glm::translate({}, glm::vec3{ randX, 0.0f, randZ }));
	}

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
	m_networkSystem->beginFrame();

	// Update all the entities using all the systems.
	for (size_t entityID = 0; entityID < SceneUtils::getEntityCount(m_scene); ++entityID) {
		m_inputSystem.update(entityID);
		m_movementSystem.update(entityID);
		m_networkSystem->update(entityID);
		m_renderSystem.update(entityID);
	}

	// Do operations that should happen at the end of the frame.
	m_renderSystem.endRender();

	glfwPollEvents();
}
