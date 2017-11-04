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
	PlayerStatsMenu(Scene& scene);
	~PlayerStatsMenu();

	// Renders the stats of all the connected players in the stats menu
	void renderStats();

	virtual void handleLobbyUpdate(const std::vector<PlayerInfo>& playerInfo) override;
	virtual void handleBroadcastResponse(const std::string& serverName, const sockaddr_in& serverAddress);
	virtual void handleJoinAccepted();
	virtual void handleJoinRejected();
private:

	Scene& m_scene;
	std::vector<TextLabel> m_statsScreenLabels;
};

