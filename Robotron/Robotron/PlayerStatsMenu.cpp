#include "PlayerStatsMenu.h"



PlayerStatsMenu::PlayerStatsMenu(Scene& scene)
	:m_scene(scene)
{
}


PlayerStatsMenu::~PlayerStatsMenu()
{
}

void PlayerStatsMenu::updateStats()
{
	int playerNum = 0;
	// If no text labels exist, populate them
	if (m_statsScreenLabels.size() == 0)
		populateTextLabels();

	// Cycle over all objects in the scene and find the player object
	for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
	{
		// Update all the players stats in their text label
		if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER))
		{
			m_statsScreenLabels.at(playerNum).setText((m_scene.getEntity(i).playerStats.playerInfo.username)
				+ " Lives: " + std::to_string(m_scene.getEntity(i).playerStats.playerInfo.lives)
				+ " Score: " + std::to_string(m_scene.getEntity(i).playerStats.playerInfo.score));
			++playerNum;
		}
	}
}

void PlayerStatsMenu::renderStats()
{
	for (size_t i = 0; i < m_statsScreenLabels.size(); ++i)
		m_statsScreenLabels.at(i).Render();
}

void PlayerStatsMenu::populateTextLabels()
{
	int numOfPlayers = 0;
	// Cycle over all the entities in the scene and find the total number of players
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i)
	{
		if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER))
			++numOfPlayers;
	}
	// Create text labels for each player
	for (size_t i = 0; i < numOfPlayers; ++i)
	{

		TextLabel playerInfo("Player", "Assets/Fonts/NYCTALOPIATILT.TTF");
		playerInfo.setScale(0.3f);
		playerInfo.setPosition(glm::vec2(1030.0f, 770.0f - (i * 15)));
		playerInfo.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
		m_statsScreenLabels.push_back(playerInfo);
	}
}
