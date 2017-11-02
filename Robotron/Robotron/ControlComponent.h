//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Per entity movement settings.
// Author       : Lance Chaney Jack Mair
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

struct ControlComponent {
	float maxMoveSpeed;
	float maxAcceleration;    // This sets the maximum turning / acceleration speed
	float damping;
	float accelerationWeight; // This scales how fast the entity will turn / accelerate
	float orientationSensitivity;
	bool worldSpaceMove;
	double lastFiringTime; // Keeps track of the last time the entity fired
	double firingSpeed; // The time allowed before the entity can fire again
};