#include "Level.h"

#include "NetworkClientSystem.h"

#include <iostream>
#include <string>

Level::Level(GLFWwindow* window, int levelNum)
	: m_scene{}
	, m_renderSystem(window, m_scene)
	, m_playerControlSystem(m_scene)
	, m_inputSystem(window, m_scene)
	, m_enemy01ControlSystem(m_scene)
	, m_enemy02ControlSystem(m_scene)
	, m_enemy03ControlSystem(m_scene)
{

	m_pClock.Process();

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

	Entity& player = EntityUtils::createPlayer(m_scene,
		glm::translate({}, glm::vec3{ 0.0f, 1.0f, 0.0f }));
	player.componentMask |= COMPONENT_NETWORK;

	EntityUtils::createModel(m_scene, "Assets/Models/nanosuit/nanosuit.obj", 
		  glm::translate({}, glm::vec3{ 1.0f, 10.0f, 10.0f })
		* glm::scale({}, glm::vec3{ 1.0f, 1.0f, 1.0f }));

	// Create the number of enemy01 based on current level with some random variance.
	unsigned int numberOfEnemy01 = 2 + m_levelNum + randomInt(-2, 2);
	
	for (unsigned int i = 0; i <= numberOfEnemy01; ++i)
	{
		float randX = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randX += 25;
	
		float randZ = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randZ += 25;

		Entity& enemy = EntityUtils::createEnemy01(m_scene,
			glm::translate({}, glm::vec3{ randX, 0.0f, randZ }));
		enemy.componentMask |= COMPONENT_NETWORK;
	}

	// Create enemy02s
	//for (unsigned int i = 0; i <= 10; ++i)
	//{
	//	float randX = randomReal<float>(-20.0f, -5.0f);
	//	if (randomInt(0, 1) == 0)
	//		randX += 25;
	//
	//	float randZ = randomReal<float>(-20.0f, -5.0f);
	//	if (randomInt(0, 1) == 0)
	//		randZ += 25;
	//
	//	EntityUtils::createEnemy02(m_scene,
	//		glm::translate({}, glm::vec3{ randX, 0.0f, randZ }), i);
	//}

	// Create enemy03s
	//for (unsigned int i = 0; i <= 5; ++i)
	//{
	//	float randX = randomReal<float>(-20.0f, -5.0f);
	//	if (randomInt(0, 1) == 0)
	//		randX += 25;
	//
	//	float randZ = randomReal<float>(-20.0f, -5.0f);
	//	if (randomInt(0, 1) == 0)
	//		randZ += 25;
	//
	//	EntityUtils::createEnemy03(m_scene,
	//		glm::translate({}, glm::vec3{ randX, 0.0f, randZ }));
	//}

	// Create the skybox
	Entity& skybox = EntityUtils::createSkybox(m_scene, {
		"Assets/Textures/envmap_violentdays/violentdays_rt.tga",
		"Assets/Textures/envmap_violentdays/violentdays_lf.tga",
		"Assets/Textures/envmap_violentdays/violentdays_up.tga",
		"Assets/Textures/envmap_violentdays/violentdays_dn.tga",
		"Assets/Textures/envmap_violentdays/violentdays_bk.tga",
		"Assets/Textures/envmap_violentdays/violentdays_ft.tga",
	});

	Texture irradianceMap = GLUtils::loadCubeMap({
		"Assets/Textures/envmap_violentdays/violentdays_irr_c00.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c01.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c02.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c03.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c04.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c05.bmp",
	});

	// Set skybox as environment map for reflections
	// The skybox only has one colormap texture so use this as the reflection map.
	m_renderSystem.setReflectionMap(skybox.model.materials.at(0).colorMaps.at(0).id);
	m_renderSystem.setIrradianceMap(irradianceMap.id);

	// Setup the camera
	Entity& cameraEntity = EntityUtils::createCamera(m_scene, { 0, 40, 20 }, { 0, 0, 0 }, { 0, 1, 0 });
	m_renderSystem.setCamera(&cameraEntity);
}


Level::~Level()
{
}

void Level::executeOneFrame()
{
	float fDT = m_pClock.GetDeltaTick();
	
	process(fDT);

	m_pClock.Process();

	Sleep(1);
}

void Level::process(float deltaTick)
{
	// Do any operations that should only happen once per frame.
	m_inputSystem.beginFrame();
	m_renderSystem.beginRender();
	m_networkSystem->beginFrame();

	// Update all the entities using all the systems.
	for (size_t i = 0; i < m_scene.entities.size(); ++i) {
		m_inputSystem.update(*m_scene.entities.at(i));
		m_playerControlSystem.update(*m_scene.entities.at(i));
		m_networkSystem->update(*m_scene.entities.at(i));
		m_renderSystem.update(*m_scene.entities.at(i));
		m_enemy01ControlSystem.update(*m_scene.entities.at(i), deltaTick);
		m_enemy02ControlSystem.update(*m_scene.entities.at(i), deltaTick);
		m_enemy03ControlSystem.update(*m_scene.entities.at(i), deltaTick);
	}

	// Do operations that should happen at the end of the frame.
	m_renderSystem.endRender();

	glfwPollEvents();
}
