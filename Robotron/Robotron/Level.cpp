#include "Level.h"

#include "NetworkClientSystem.h"

#include <iostream>
#include <string>

Level::Level(GLFWwindow* window, int levelNum)
	: m_scene{}
	, m_renderSystem(window, m_scene)
	, m_movementSystem(m_scene)
	, m_inputSystem(window, m_scene)
{
	m_window = window;
	m_levelNum = levelNum;

	std::string strServerMode;
	std::cout << "Run in server mode: ";
	std::cin >> strServerMode;
	if (strServerMode == "y")
		m_networkSystem = std::make_unique<NetworkServerSystem>(m_scene);
	else
		m_networkSystem = std::make_unique<NetworkClientSystem>(m_scene);

	// Create 3D entities.
	EntityUtils::createQuad(m_scene,
		glm::rotate(glm::mat4{}, static_cast<float>(M_PI / 2), glm::vec3{ 1, 0, 0 })
		* glm::scale({}, glm::vec3{ 20.0f, 20.0f, 1.0f }));

	EntityUtils::createPlayer(m_scene,
		glm::translate({}, glm::vec3{ 0.0f, 1.0f, 0.0f }));

	//create the number of enemy01 based on current level with some random variance.
	unsigned int numberOfEnemy01 = 9 + m_levelNum + randomInt(-2, 2);
	
	for (unsigned int i = 0; i <= numberOfEnemy01; ++i)
	{
		float randX = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randX += 25;

		float randZ = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randZ += 25;

		EntityUtils::createEnemy01(m_scene,
			glm::translate({}, glm::vec3{ randX, 0.0f, randZ }));
	}

	// Create the skybox
	Entity& skybox = EntityUtils::createSkybox(m_scene, {
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
	Entity& cameraEntity = EntityUtils::createCamera(m_scene, { 0, 40, 20 }, { 0, 0, 0 }, { 0, 1, 0 });
	m_renderSystem.setCamera(&cameraEntity);
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
	for (size_t i = 0; i < m_scene.entities.size(); ++i) {
		m_inputSystem.update(m_scene.entities.at(i));
		m_movementSystem.update(m_scene.entities.at(i));
		m_networkSystem->update(m_scene.entities.at(i));
		m_renderSystem.update(m_scene.entities.at(i));
	}

	// Do operations that should happen at the end of the frame.
	m_renderSystem.endRender();

	glfwPollEvents();
}
