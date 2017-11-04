#include "PlayerStatsMenu.h"



PlayerStatsMenu::PlayerStatsMenu(Scene& scene)
	:m_scene(scene)
{
}


PlayerStatsMenu::~PlayerStatsMenu()
{
}

void PlayerStatsMenu::renderStats()
{
	for (size_t i = 0; i < m_statsScreenLabels.size(); ++i)
		m_statsScreenLabels.at(i).Render();
}

void PlayerStatsMenu::handleBroadcastResponse(const std::string& serverName, const sockaddr_in& serverAddress)
{
}

void PlayerStatsMenu::handleJoinAccepted()
{
}

void PlayerStatsMenu::handleJoinRejected()
{
}

void PlayerStatsMenu::handleLobbyUpdate(const std::vector<PlayerInfo>& playerInfo)
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

	// Cycle over all objects in the scene and find the player object
	for (unsigned int i = 0; i < playerInfo.size(); ++i)
	{
		m_statsScreenLabels.at(i).setText((playerInfo.at(i).username)
			+ " Lives: " + std::to_string(playerInfo.at(i).getLives())
			+ " Score: " + std::to_string(playerInfo.at(i).getScore()));
	}
}