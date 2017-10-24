#include "Level.h"

#include "NetworkClientSystem.h"

#include <iostream>
#include <string>

Level::Level(GLFWwindow* window)
	: m_scene{}
	, m_renderSystem(window, m_scene)
	, m_playerControlSystem(m_scene)
	, m_inputSystem(window, m_scene)
	, m_enemy01ControlSystem(m_scene)
	, m_enemy02ControlSystem(m_scene)
	, m_enemy03ControlSystem(m_scene)
	, m_scorePickUpSystem(m_scene)
	, m_playerbulletsystem(m_scene)
{

	m_pClock.Process();

	m_window = window;
	m_levelNum = 0;

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

	// Create the skybox
	Entity& skybox = EntityUtils::createSkybox(m_scene, {
		"Assets/Textures/envmap_violentdays/violentdays_rt.tga",
		"Assets/Textures/envmap_violentdays/violentdays_lf.tga",
		"Assets/Textures/envmap_violentdays/violentdays_up.tga",
		"Assets/Textures/envmap_violentdays/violentdays_dn.tga",
		"Assets/Textures/envmap_violentdays/violentdays_bk.tga",
		"Assets/Textures/envmap_violentdays/violentdays_ft.tga",
	});

	Texture irradianceMap = GLUtils::loadCubeMapFaces({
		"Assets/Textures/envmap_violentdays/violentdays_irr_c00.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c01.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c02.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c03.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c04.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c05.bmp",
	});

	Texture radianceMap = GLUtils::loadDDSTexture("Assets/Textures/envmap_violentdays/violentdays_pmrem.dds");

	// Set skybox as environment map for reflections
	// The skybox only has one colormap texture so use this as the reflection map.
	m_renderSystem.setRadianceMap(radianceMap.id);
	m_renderSystem.setIrradianceMap(irradianceMap.id);

	// Setup the camera
	Entity& cameraEntity = EntityUtils::createCamera(m_scene, { 0, 40, 20 }, { 0, 0, 0 }, { 0, 1, 0 });
	m_renderSystem.setCamera(&cameraEntity);
}


Level::~Level()
{
}

void Level::spawnEnemies(int levelType)
{
	// Different combinations of enemies are spawned based on the level type.
	// The number of enemies increases as the level number increases.
	
	int zombieCount = 0;
	int snakePartsCount = 0;
	int shooterCount = 0;

	int score1PickUpCount = m_levelNum + randomInt(0, 2);
	int score2PickUpCount = m_levelNum + randomInt(-4, -2);
	int healthPickUpCount = m_levelNum + randomInt(-4, -2);


	// Spawn Zombies level type
	if (levelType == 0)
	{
		zombieCount = 2 + m_levelNum + randomInt(0, 2);
	}
	// Spawn mixed level type
	else if (levelType == 1)
	{
		zombieCount = m_levelNum + randomInt(1, 2);
		shooterCount = m_levelNum + randomInt(-4, -2);
	}
	// Spawn shooter level type
	else if (levelType == 2)
	{
		shooterCount = 2 + m_levelNum + randomInt(0, 2);
	}
	// Spawn snake level type
	else if (levelType == 3)
	{
		snakePartsCount = 2 + m_levelNum + randomInt(0, 2);
	}
	// Spawn bonus level type
	else if (levelType == 4)
	{
		zombieCount = randomInt(1, 3);
		score1PickUpCount += randomInt(4, 8);
		score2PickUpCount += randomInt(2, 6);
		healthPickUpCount += randomInt(1, 3);
	}

	//EntityUtils::createModel(m_scene, "Assets/Models/nanosuit/nanosuit.obj", 
	//	  glm::translate({}, glm::vec3{ 1.0f, 10.0f, 10.0f })
	//	* glm::scale({}, glm::vec3{ 1.0f, 1.0f, 1.0f }));

	// Create all the zombie enemy types in the scene.
	for (int i = 0; i < zombieCount; ++i)
	{
		float randX = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randX += 25;

		float randZ = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randZ += 25;

		Entity& enemy = EntityUtils::createEnemy01(m_scene,
			glm::translate({}, glm::vec3{ randX, 1.0f, randZ }));
		enemy.componentMask |= COMPONENT_NETWORK;
	}

	// Create all the snake enemy types in the scene.
	for (int i = 0; i < snakePartsCount; ++i)
	{
		float randX = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randX += 25;
	
		float randZ = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randZ += 25;
	
		EntityUtils::createEnemy02(m_scene,
			glm::translate({}, glm::vec3{ randX, 1.0f, randZ }), i);
	}

	// Create all the shooter enemy types in the scene.
	for (int i = 0; i < shooterCount; ++i)
	{
		float randX = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randX += 25;
	
		float randZ = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randZ += 25;
	
		EntityUtils::createEnemy03(m_scene,
			glm::translate({}, glm::vec3{ randX, 1.0f, randZ }));
	}

	// Create ScorePickups01
	for (int i = 0; i < score1PickUpCount; ++i)
	{
		float randX = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randX += 25;
	
		float randZ = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randZ += 25;
	
		Entity& enemy = EntityUtils::createScorePickUp01(m_scene,
			glm::translate({}, glm::vec3{ randX, 1.0f, randZ })
			* glm::scale({}, glm::vec3{ 0.5f, 0.5f, 0.5f }));
	}

	// Create ScorePickups02
	for (int i = 0; i < score2PickUpCount; ++i)
	{
		float randX = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randX += 25;

		float randZ = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randZ += 25;

		Entity& enemy = EntityUtils::createScorePickUp02(m_scene,
			glm::translate({}, glm::vec3{ randX, 1.0f, randZ })
			* glm::scale({}, glm::vec3{ 0.5f, 0.5f, 0.5f }));
	}

	// Create Health Pickups
	for (int i = 0; i < healthPickUpCount; ++i)
	{
		float randX = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randX += 25;

		float randZ = randomReal<float>(-20.0f, -5.0f);
		if (randomInt(0, 1) == 0)
			randZ += 25;

		Entity& enemy = EntityUtils::createHealthPickUp(m_scene,
			glm::translate({}, glm::vec3{ randX, 1.0f, randZ })
			* glm::scale({}, glm::vec3{ 0.5f, 0.5f, 0.5f }));
	}
}

void Level::initalizeNextLevel()
{
	++m_levelNum;
	
	int randomNum;
	
	// Only spawn zombies or mixed below level 5.
	if (m_levelNum < 5)
		randomNum = randomInt(0, 1);
	
	// Spawn all enemy types above level 5.
	else if (m_levelNum % 5 != 0)
		randomNum = randomInt(0, 3);
	
	// A bonus level occurs every 5 levels.
	else
		randomNum = 4;

	// Delete all score pickups and update the players score.
	for (unsigned int i = 0; i < m_scene.entities.size(); ++i)
	{
		if ((m_scene.entities.at(i)->componentMask & COMPONENT_SCOREPICKUP) == COMPONENT_SCOREPICKUP)
		{
			// Increase the player's score value.
			if (m_scene.entities.at(i)->aiVariables.followEntity != NULL)
			{
				if (m_scene.entities.at(i)->aiVariables.lifePickUp != true)
					m_scene.entities.at(i)->aiVariables.followEntity->playerStats.score += m_scene.entities.at(i)->aiVariables.score;
				else
					++m_scene.entities.at(i)->aiVariables.followEntity->playerStats.lives;
			}
			m_scene.destroyEntity(*m_scene.entities.at(i));
		}
	}

	spawnEnemies(randomNum);
}

bool Level::checkEnemiesAlive()
{
	// Cycle through all the entites in the scene.
	for (unsigned int i = 0; i < m_scene.entities.size(); ++i)
	{
		// Return true when the first entity is found with an enemy tag.
		if ((m_scene.entities.at(i)->componentMask & COMPONENT_ENEMY01) == COMPONENT_ENEMY01
		 || (m_scene.entities.at(i)->componentMask & COMPONENT_ENEMY02) == COMPONENT_ENEMY02
		 || (m_scene.entities.at(i)->componentMask & COMPONENT_ENEMY03) == COMPONENT_ENEMY03)
			return true;
	}

	return false;
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
		m_scorePickUpSystem.update(*m_scene.entities.at(i), deltaTick);
		m_playerbulletsystem.update(*m_scene.entities.at(i), deltaTick);
	}

	// Do operations that should happen at the end of the frame.
	m_renderSystem.endRender();

	glfwPollEvents();
}
