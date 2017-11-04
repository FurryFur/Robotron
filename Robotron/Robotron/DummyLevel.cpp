#define _USE_MATH_DEFINES

#include "DummyLevel.h"
#include "Utils.h"
#include "GLUtils.h"
#include "EntityUtils.h"

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

//DummyLevel::DummyLevel(GLFWwindow* window, Clock& clock, Scene& scene, std::string username, std::uint8_t playerIDNum)
DummyLevel::DummyLevel(GLFWwindow* window, Clock& clock, Scene& scene, std::string username)
	: m_scene(scene)
	, m_renderSystem(window, m_scene)
	, m_inputSystem(window, m_scene, clock)
	, m_playerScore("Score: ", "Assets/Fonts/NYCTALOPIATILT.TTF")
	, m_playerHealth("Health: ", "Assets/Fonts/NYCTALOPIATILT.TTF")
	, m_playerStatsMenu(m_scene)
	, m_physicsSystem(m_scene)
{
	Scene::makeSceneCurrent(&m_scene);
	m_inputSystem.registerKeyObserver(this);

	m_window = window;
	//m_playerIDNum = playerIDNum;
	// Set the UI position, scale, colour
	m_playerHealth.setPosition(glm::vec2(10.0f, 10.0f));
	m_playerHealth.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_playerHealth.setScale(0.5f);

	m_playerScore.setPosition(glm::vec2(10.0f, 40.0f));
	m_playerScore.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_playerScore.setScale(0.5f);

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


DummyLevel::~DummyLevel()
{
}

void DummyLevel::process(float deltaTick, Clock& clock, NetworkSystem& networkSystem)
{
	// Cycle over all objects in the scene and find the player object
	for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER) 
		 && m_scene.getEntity(i).player.playerInfo->getPlayerID() == m_playerIDNum)
		{
			// Update the UI with the player score and health.
			m_playerHealth.setText("Health: " + std::to_string(m_scene.getEntity(i).player.playerInfo->getLives()));
			m_playerScore.setText("Score: " + std::to_string(m_scene.getEntity(i).player.playerInfo->getScore()));
		}
	}

	// Do any operations that should only happen once per frame.
	m_inputSystem.beginFrame();
	m_renderSystem.beginRender();
	networkSystem.beginFrame();

	// Update all the entities using all the systems.
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& entity = m_scene.getEntity(i);
		m_inputSystem.update(entity);
		m_physicsSystem.update(entity, deltaTick);
		networkSystem.update(entity, deltaTick);
		m_renderSystem.update(entity);
	}

	// Draw all the player stats only if the flag is set to true.
	if (m_drawConnectPlayerStats)
	{
		m_playerStatsMenu.updateStats();
		m_playerStatsMenu.renderStats();
	}

	m_playerScore.Render();
	m_playerHealth.Render();

	// Do operations that should happen at the end of the frame.
	m_renderSystem.endRender();
	networkSystem.endFrame();
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
	// Cycle through all the entites in the scene.
	for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER) && m_scene.getEntity(i).player.playerInfo->getLives() > 0)
			return true;
	}

	return false;
}

int DummyLevel::getPlayerScore()
{
	// Cycle through all the entites in the scene.
	for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER_CONTROL)
		 && m_scene.getEntity(i).player.playerInfo->getPlayerID() == m_playerIDNum)
			return m_scene.getEntity(i).player.playerInfo->getScore();
	}
	return 0;
}