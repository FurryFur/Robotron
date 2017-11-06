#define _USE_MATH_DEFINES

#include "DummyLevel.h"
#include "Utils.h"
#include "GLUtils.h"
#include "EntityUtils.h"

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

//DummyLevel::DummyLevel(GLFWwindow* window, Clock& clock, Scene& scene, std::string username, std::uint8_t playerIDNum)
DummyLevel::DummyLevel(GLFWwindow* window, Clock& clock, Scene& scene, std::string username, std::uint8_t playerID, NetworkSystem& networkSystem, const std::vector<PlayerInfo>& playerInfo)
	: m_scene(scene)
	, m_networkSystem(networkSystem)
	, m_renderSystem(window, m_scene)
	, m_inputSystem(window, m_scene, clock)
	, m_playerStatsMenu(m_scene, playerID, playerInfo)
	, m_physicsSystem(m_scene)
{
	Scene::makeSceneCurrent(&m_scene);
	m_inputSystem.registerKeyObserver(this);
	m_playerID = playerID;

	m_networkSystem.registerEntityEventListener(&m_playerStatsMenu);

	m_window = window;
	// Set the UI position, scale, colour

	// Create 3D entities.
	TransformComponent transform{};
	transform.eulerAngles.x = static_cast<float>(M_PI / 2);
	transform.scale = glm::vec3{ 20.0f, 20.0f, 20.0f };
	EntityUtils::createQuad(m_scene, transform);

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
	Entity& cameraEntity = EntityUtils::createCamera(m_scene, { 0, 23, 26 }, { 0, 0, 5 }, { 0, 1, 0 });
	m_renderSystem.setCamera(&cameraEntity);
}


DummyLevel::~DummyLevel()
{
	m_networkSystem.removeEntityEventListener(&m_playerStatsMenu);
}

void DummyLevel::process(float deltaTick, Clock& clock)
{
	// Update the flash light colour for the player depending if they are on their respawn timer or not.
	for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).player.lastSpawnTime == 0
		 || (m_scene.getEntity(i).transform.position.y >= 48.5f && m_scene.getEntity(i).transform.position.y <= 50.5f))
			m_scene.getEntity(i).player.lastSpawnTime = clock.GetCurTime();
		
		if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER, COMPONENT_SPOTLIGHT)
		&& m_scene.getEntity(i).player.playerInfo.getPlayerID() == m_playerID)
		{
			// The actual lives are less than the lives tracker
			if (m_scene.getEntity(i).player.playerInfo.getLives() < m_livesTracker)
			{
				m_scene.getEntity(i).player.lastSpawnTime = clock.GetCurTime() + m_scene.getEntity(i).player.respawnTime;
				m_livesTracker = m_scene.getEntity(i).player.playerInfo.getLives();
			}
			// The actual lives are more than the lives tracker
			else if (m_scene.getEntity(i).player.playerInfo.getLives() > m_livesTracker)
				m_livesTracker = m_scene.getEntity(i).player.playerInfo.getLives();

			if (clock.GetCurTime() <= (m_scene.getEntity(i).player.invunTimer + (m_scene.getEntity(i).player.lastSpawnTime)))
			{
				m_scene.getEntity(i).spotlight.color = glm::vec3(0.0f, 5.6f, 0.0f);
			}
			else
				m_scene.getEntity(i).spotlight.color = glm::vec3(0.5f, 0.75f, 1.5f) * 4.0f;

			break;
		}
	}
	
	// Do any operations that should only happen once per frame.
	m_inputSystem.beginFrame();
	m_renderSystem.beginRender();

	// Update all the entities using all the systems.
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& entity = m_scene.getEntity(i);
		m_inputSystem.update(entity);
		m_physicsSystem.update(entity, deltaTick);
		m_networkSystem.update(entity, deltaTick);
		m_renderSystem.update(entity);
	}

	// Draw all the player stats only if the flag is set to true.
	if (m_drawConnectPlayerStats)
	{
		//m_playerStatsMenu.updateStats();
			m_playerStatsMenu.renderStats();
	}
	m_playerStatsMenu.renderUI();
	// Do operations that should happen at the end of the frame.
	m_renderSystem.endRender();
}

void DummyLevel::keyCallback(int key, int scancode, int action, int mods)
{
	// Tab key was pressed draw/stop drawing the player stats menu
	if (key == 258) {
		if (action == GLFW_PRESS)
		{
			if (m_drawConnectPlayerStats == true)
				m_drawConnectPlayerStats = false;
			else
				m_drawConnectPlayerStats = true;
		}
		return;
	}
}

// Check atleast one player is still alive. If return false, trigger the game to end.
bool DummyLevel::checkPlayersAlive()
{
	return m_playerStatsMenu.checkPlayersAlive();
}

int DummyLevel::getPlayerScore()
{
	// Cycle through all the entites in the scene.
	for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER)
		 && m_scene.getEntity(i).player.playerInfo.getPlayerID() == m_playerID)
			return m_scene.getEntity(i).player.playerInfo.getScore();
	}
	return 0;
}