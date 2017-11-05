//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Controls the player stats menu that can be toggled to display in the level
//				  
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#pragma once

#include "TextLabel.h"
#include "Scene.h"
#include "LobbyEventListener.h"

#include <vector>

class PlayerStatsMenu : public LobbyEventListener
{
public:
	PlayerStatsMenu(Scene& scene, std::uint8_t playerID);
	~PlayerStatsMenu();

	void renderStats(); // Renders the stats of all the connected players in the stats menu
	void renderUI(); // Renders the players menu with their personal score/health
	bool checkPlayersAlive(); // Checks if all the players are dead

	std::uint8_t m_playerID;
	virtual void handleLobbyUpdate(const std::vector<PlayerInfo>& playerInfo) override;
	virtual void handleBroadcastResponse(const std::string& serverName, const sockaddr_in& serverAddress) override;
	virtual void handleJoinAccepted() override;
	virtual void handleJoinRejected() override;
	virtual void handleGameStart() override;
	virtual void handleDisconnect() override;
private:
	bool platersConnected; // A flag to check the players are properly connected before checking if they are dead
	bool m_playersAlive;
	Scene& m_scene;
	std::vector<TextLabel> m_statsScreenLabels;

	TextLabel m_playerHealth;
	TextLabel m_playerScore;
};

