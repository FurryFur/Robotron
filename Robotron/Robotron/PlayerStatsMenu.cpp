#include "PlayerStatsMenu.h"



PlayerStatsMenu::PlayerStatsMenu(Scene& scene, std::uint8_t playerID)
	:m_scene(scene)
	, m_playerScore("Score: 0", "Assets/Fonts/NYCTALOPIATILT.TTF")
	, m_playerHealth("", "Assets/Fonts/NYCTALOPIATILT.TTF")
{
	m_playerID = playerID;

	m_playerHealth.setPosition(glm::vec2(10.0f, 10.0f));
	m_playerHealth.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_playerHealth.setScale(0.5f);

	m_playerScore.setPosition(glm::vec2(10.0f, 40.0f));
	m_playerScore.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_playerScore.setScale(0.5f);

	m_playersAlive = true;

	for (size_t i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER))
		{
			m_playerHealth.setText("Health: " + std::to_string(m_scene.getEntity(i).player.playerInfo.getLives()));
			TextLabel playerInfo(m_scene.getEntity(i).player.playerInfo.username
				+" Lives: " + std::to_string(m_scene.getEntity(i).player.playerInfo.getLives())
				+ " Score: " + std::to_string(m_scene.getEntity(i).player.playerInfo.getScore())				
				, "Assets/Fonts/NYCTALOPIATILT.TTF");
			playerInfo.setScale(0.3f);
			playerInfo.setPosition(glm::vec2(1030.0f, 770.0f - (i * 15)));
			playerInfo.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
			m_statsScreenLabels.push_back(playerInfo);
		}
	}
}

PlayerStatsMenu::~PlayerStatsMenu()
{
}

void PlayerStatsMenu::renderStats()
{
	for (size_t i = 0; i < m_statsScreenLabels.size(); ++i)
		m_statsScreenLabels.at(i).Render();
}

void PlayerStatsMenu::renderUI()
{
	m_playerScore.Render();
	m_playerHealth.Render();
}

bool PlayerStatsMenu::checkPlayersAlive()
{
	return m_playersAlive;
}

void PlayerStatsMenu::onBroadcastResponse(const std::string& serverName, const sockaddr_in& serverAddress)
{
}

void PlayerStatsMenu::onJoinAccepted()
{
}

void PlayerStatsMenu::onJoinRejected()
{
}

void PlayerStatsMenu::onGameStart()
{
}

void PlayerStatsMenu::onDisconnect()
{
}

bool checkAlive(const std::vector<PlayerInfo>& playerInfo)
{
	for (unsigned int i = 0; i < playerInfo.size(); ++i)
	{
		if (playerInfo.at(i).getLives() != 255
		 && playerInfo.at(i).getLives() > 0)
			return true;
	}
	return false;
}

void PlayerStatsMenu::onPlayersUpdated(const std::vector<PlayerInfo>& playerInfo)
{
	// If no text labels exist, populate them
	if (m_statsScreenLabels.size() != playerInfo.size())
	{
		m_statsScreenLabels.clear();
		for (size_t i = 0; i < playerInfo.size(); ++i)
		{

			TextLabel playerInfo("Player", "Assets/Fonts/NYCTALOPIATILT.TTF");
			playerInfo.setScale(0.3f);
			playerInfo.setPosition(glm::vec2(1030.0f, 770.0f - (i * 15)));
			playerInfo.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
			m_statsScreenLabels.push_back(playerInfo);
		}
	}

	for (unsigned int i = 0; i < playerInfo.size(); ++i)
	{
		// Update the tab screen stats
		m_statsScreenLabels.at(i).setText((playerInfo.at(i).username)
			+ " Lives: " + std::to_string(playerInfo.at(i).getLives())
			+ " Score: " + std::to_string(playerInfo.at(i).getScore()));

		// Cycle over all objects in the scene and find the player object
		if (playerInfo.at(i).getPlayerID() == m_playerID)
		{
			// Update the UI with the player score and health.
			if (playerInfo.at(i).getLives() != 255)
				m_playerHealth.setText("Health: " + std::to_string(playerInfo.at(i).getLives()));
			else
				m_playerHealth.setText("Health: 0");

			m_playerScore.setText("Score: " + std::to_string(playerInfo.at(i).getScore()));
		}
	}

	if (!platersConnected)
	{
		for (unsigned int i = 0; i < playerInfo.size(); ++i)
		{
			if (playerInfo.at(i).getLives() > 0
			 && playerInfo.at(i).getLives() != 255)
				platersConnected = true;
		}
	}
	else {
		m_playersAlive = checkAlive(playerInfo);
	}
}