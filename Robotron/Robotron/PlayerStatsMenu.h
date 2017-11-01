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

#include <vector>

class PlayerStatsMenu
{
public:
	PlayerStatsMenu(Scene& scene);
	~PlayerStatsMenu();

	// Updates the stats of all the connected players in the stats menu
	void updateStats();
	// Renders the stats of all the connected players in the stats menu
	void renderStats();

private:
	// Adds text labels equal to the number of players
	void populateTextLabels();

	Scene& m_scene;
	std::vector<TextLabel> m_statsScreenLabels;
};

