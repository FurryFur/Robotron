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
	int score; // The current score of the player
	int extraLifeTrack; // Keeps track of the players score until they get a new life. Refreshes when they do.
	int extraLifeThreshhold; // How many points the player needs to get another life
	int lives; // The current number of lives the player has
	bool isRespawning; // Is true when they are dead and will respawn
	double deathTime; // The time it takes for them to respawn
	double lastFiringTime; // Keeps track of the last time the entity fired
	double firingSpeed; // The time allowed before the entity can fire again
};
