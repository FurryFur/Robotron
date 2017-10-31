#include "Level.h"

#include "NetworkClientSystem.h"

#include <iostream>
#include <string>

Level::Level(GLFWwindow* window, Clock& clock)
	: m_scene{}
	, m_renderSystem(window, m_scene)
	, m_playerControlSystem(m_scene)
	, m_inputSystem(window, m_scene, clock)
	, m_enemy01ControlSystem(m_scene)
	, m_enemy02ControlSystem(m_scene)
	, m_enemy03ControlSystem(m_scene)
	, m_scorePickUpSystem(m_scene)
	, m_playerbulletsystem(m_scene)
	, m_enemybulletsystem(m_scene)
	, m_physicsSystem(m_scene)
	, m_playerScore("Score: ", "Assets/Fonts/NYCTALOPIATILT.TTF")
	, m_playerHealth("Health: ", "Assets/Fonts/NYCTALOPIATILT.TTF")
{
	Scene::makeSceneCurrent(&m_scene);

	m_window = window;
	m_levelNum = 0;
	m_inSetupPhase = false;
	m_descendingPlayers = true;

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
		glm::translate({}, glm::vec3{ 0.0f, 50.0f, 0.0f }));
	player.addComponents(COMPONENT_NETWORK);

	spawnEnemies(0);

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

	// Set the UI position, scale, colour
	m_playerHealth.setPosition(glm::vec2(10.0f, 10.0f));
	m_playerHealth.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_playerHealth.setScale(0.5f);

	m_playerScore.setPosition(glm::vec2(10.0f, 40.0f));
	m_playerScore.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_playerScore.setScale(0.5f);
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
	int healthPickUpCount = 0;

	// Start off with basic levels
	if (m_levelNum == 0)
	{
		zombieCount = 2;

	}
	else if (m_levelNum == 1)
	{
		zombieCount = 3;
		score1PickUpCount = 2;
	}
	else if (m_levelNum == 2)
	{
		snakePartsCount = 8 + m_levelNum + randomInt(0, 2);
		zombieCount = 3;
		score1PickUpCount = 4;
		int healthPickUpCount = 1;
	}
	// Spawn Zombies level type
	else if (levelType == 0)
	{
		zombieCount = 4 + m_levelNum + randomInt(0, 2);
	}
	// Spawn mixed level type
	else if (levelType == 1)
	{
		zombieCount = m_levelNum + randomInt(1, 2);
		shooterCount = randomInt(0, 2);
		snakePartsCount = 8 + m_levelNum + randomInt(-4, 2);
	}
	// Spawn shooter level type
	else if (levelType == 2)
	{
		shooterCount = 2 + randomInt(0, 2);
		zombieCount = m_levelNum + randomInt(0, 2);
	}
	// Spawn snake level type
	else if (levelType == 3)
	{
		zombieCount = m_levelNum/2 + randomInt(0, 2);
		snakePartsCount = 16 + m_levelNum/2 + randomInt(0, 2);
	}
	// Spawn bonus level type
	else if (levelType == 4)
	{
		zombieCount = randomInt(1, 3);
		score1PickUpCount += randomInt(4, 8);
		score2PickUpCount += randomInt(2, 6);
		healthPickUpCount += randomInt(0, 1);
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

		Entity& enemy = EntityUtils::createEnemyZombie(m_scene,
			glm::translate({}, glm::vec3{ randX, 1.0f, randZ }));
		enemy.addComponents(COMPONENT_NETWORK);
	}


	// Create all the snake enemy types in the scene.
	int rand = randomInt(0, 3);
	float randcoord = randomReal<float>(9.0f, 17.0f);
	if (rand == 0) {
		for (int i = 0; i < snakePartsCount; ++i)
		{
			if (-19.0f + i < 20.0f)
			{
				EntityUtils::createEnemySnake(m_scene,
					glm::translate({}, glm::vec3{ -(randcoord + randomReal<float>(-0.2f, 0.2f)), 1.0f, -19.0f + i }), i);
			}
			else
			{
				EntityUtils::createEnemySnake(m_scene,
					glm::translate({}, glm::vec3{ -(randcoord + randomReal<float>(-0.2f, 0.2f)), 1.0f, -19.0f}), i);
			}
		}
	}
	else if (rand == 1) {
		for (int i = 0; i < snakePartsCount; ++i)
		{

			if (19.0f - i < 20.0f)
			{
				EntityUtils::createEnemySnake(m_scene,
					glm::translate({}, glm::vec3{ -(randcoord + randomReal<float>(-0.2f, 0.2f)), 1.0f, 19.0f - i }), i);
			}
			else
			{
				EntityUtils::createEnemySnake(m_scene,
					glm::translate({}, glm::vec3{ -(randcoord + randomReal<float>(-0.2f, 0.2f)), 1.0f, 19.0f}), i);
			}
		}
	}
	else if (rand == 2) {
		for (int i = 0; i < snakePartsCount; ++i)
		{
			if (-19.0f + i < 20.0f)
			{
				EntityUtils::createEnemySnake(m_scene,
					glm::translate({}, glm::vec3{ -19.0f + i, 1.0f, (randcoord + randomReal<float>(-0.2f, 0.2f)) }), i);
			}
			else
			{
				EntityUtils::createEnemySnake(m_scene,
					glm::translate({}, glm::vec3{ -19.0f, 1.0f, (randcoord + randomReal<float>(-0.2f, 0.2f)) }), i);
			}
		}
	}
	else if (rand == 3) {
		for (int i = 0; i < snakePartsCount; ++i)
		{
			if (19.0f - i < 20.0f)
			{
				EntityUtils::createEnemySnake(m_scene,
					glm::translate({}, glm::vec3{ 19.0f - i, 1.0f, (randcoord + randomReal<float>(-0.2f, 0.2f)) }), i);
			}
			else
			{
				EntityUtils::createEnemySnake(m_scene,
					glm::translate({}, glm::vec3{ 19.0f, 1.0f, (randcoord + randomReal<float>(-0.2f, 0.2f)) }), i);
			}
		}
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
	
		EntityUtils::createEnemyShooter(m_scene,
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

	// Cycle over all the entities in the scene
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i)
	{
		// Delete all score pickups and update the players score by the ones following the player.
		if (m_scene.getEntity(i).hasComponents(COMPONENT_SCOREPICKUP))
		{
			// Increase the player's score value.
			if (m_scene.getEntity(i).aiVariables.followEntity != NULL)
			{
				if (m_scene.getEntity(i).aiVariables.lifePickUp != true)
				{
					m_scene.getEntity(i).aiVariables.followEntity->playerStats.score += m_scene.getEntity(i).aiVariables.score;
					m_scene.getEntity(i).aiVariables.followEntity->playerStats.extraLifeTrack += m_scene.getEntity(i).aiVariables.score;
				}
				else
					++m_scene.getEntity(i).aiVariables.followEntity->playerStats.lives;
			}
			m_scene.destroyEntity(m_scene.getEntity(i));
		}

		// Move the player back to the centre point of the level
		if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER_CONTROL))
		{
			m_scene.getEntity(i).transform[3].x = 0.0f;
			m_scene.getEntity(i).transform[3].z = 0.0f;
		}
	}

	// Decide the level type using a random number between 0-4.
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

	spawnEnemies(randomNum);
}

// Check atleast one enemy is alive. If return false, trigger next level to start.
bool Level::checkEnemiesAlive()
{
	// Cycle through all the entites in the scene.
	for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
	{
		// Return true when the first entity is found with an enemy tag.
		if (m_scene.getEntity(i).hasComponentsAny(COMPONENT_ZOMBIE, COMPONENT_SNAKE,
			COMPONENT_ENEMY_SHOOTER))
			return true;
	}

	return false;
}

// If enough time has passed since their death and they have lives remaining, the player is respawned.
void Level::respawnDeadPlayers(Clock& clock)
{
	// Cycle through all the entites in the scene.
	for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
	{
		// Return true when the first entity is found with an enemy tag.
		if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER_CONTROL)
		    && m_scene.getEntity(i).playerStats.isRespawning == true
		    && m_scene.getEntity(i).playerStats.lives > 0
		    && m_scene.getEntity(i).playerStats.deathTime + 3.0f <= clock.GetCurTime())
		{
			m_scene.getEntity(i).playerStats.isRespawning = false;
			m_scene.getEntity(i).transform[3] = glm::vec4{ 0.0f, 50.0f, 0.0f, 1.0f };
			m_descendingPlayers = true;
		}
	}
}

void Level::process(float deltaTick, Clock& clock)
{	
	// Cycle over all objects in the scene and find the player object
	for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER_CONTROL))
		{
			// Update the UI with the player score and health.
			m_playerHealth.setText("Health: " + std::to_string(m_scene.getEntity(i).playerStats.lives));
			m_playerScore.setText("Score: " + std::to_string(m_scene.getEntity(i).playerStats.score));

			// Check that the player has enough score to get an extra life
			if (m_scene.getEntity(i).playerStats.extraLifeTrack >= m_scene.getEntity(i).playerStats.extraLifeThreshhold)
			{
				// Reset the tracker.
				m_scene.getEntity(i).playerStats.extraLifeTrack = 0;
				// Increase the threshold by 500 every time it is reached
				m_scene.getEntity(i).playerStats.extraLifeThreshhold += 500;
				// Increase the player's life by 1.
				++m_scene.getEntity(i).playerStats.lives;
			}
		}
	}

	
	// Do any operations that should only happen once per frame.
	m_inputSystem.beginFrame();
	m_renderSystem.beginRender();

	m_networkSystem->beginFrame();
	respawnDeadPlayers(clock);
	// Update all the entities using all the systems.
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& entity = m_scene.getEntity(i);
		m_inputSystem.update(entity);
		m_playerControlSystem.update(entity, clock);
		m_enemy01ControlSystem.update(entity, deltaTick);
		m_enemy02ControlSystem.update(entity, deltaTick);
		m_enemy03ControlSystem.update(entity, deltaTick, clock);
		m_scorePickUpSystem.update(entity, deltaTick);
		m_playerbulletsystem.update(entity);
		m_enemybulletsystem.update(entity);
		m_physicsSystem.update(entity, deltaTick);
		m_networkSystem->update(entity, deltaTick);
		m_renderSystem.update(entity);
	}

	if (m_inSetupPhase)
	{
		++m_setUpTick;
		processSetUpPhase();
	}

	// When respawning, players spawn above the level.
	// This checks if they are above the level everyframe
	// and moves them one unit of space down.
	if (m_descendingPlayers == true)
	{
		m_descendingPlayers = false;
		for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
		{
			if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER_CONTROL)
			 && m_scene.getEntity(i).transform[3].y > 1.0f)
			{
				--m_scene.getEntity(i).transform[3].y;
				m_descendingPlayers = true;
			}
		}
	}

	m_playerScore.Render();
	m_playerHealth.Render();

	// Do operations that should happen at the end of the frame.
	m_renderSystem.endRender();
	m_networkSystem->endFrame();
}

// Handles the animation between levels. The player flies up, level spawns. And they desend in the centre of the screen.
void Level::processSetUpPhase()
{
	// For the first 50 ticks, ascend the players
	if (m_setUpTick <= 50)
	{
		// Cycle through all the entites in the scene.
		for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
		{
			if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER_CONTROL))
				++m_scene.getEntity(i).transform[3].y;
		}
		if(m_setUpTick == 50)
			initalizeNextLevel();
	}
	// For the last 50 ticks, descend the players
	else
	{
		// Cycle through all the entites in the scene.
		for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
		{
			if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER_CONTROL))
				--m_scene.getEntity(i).transform[3].y;
		}
		if (m_setUpTick == 100)
		{
			m_inSetupPhase = false;
			m_setUpTick = 0;
		}
	}
}

void Level::triggerNextLevel()
{
	m_inSetupPhase = true;
}