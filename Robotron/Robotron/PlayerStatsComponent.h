//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Per player health and score info.
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#pragma once

struct PlayerStatsComponent {
	int score;
	// Keeps track of the players score until they get a new life. Refreshes when they do.
	int scoreLives;
	int lives;
	bool isRespawning;
	double deathTime;
};
