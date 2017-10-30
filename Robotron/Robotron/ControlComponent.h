//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Per entity movement settings.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

struct ControlComponent {
	float maxMoveSpeed;
	float maxAcceleration;
	float orientationSensitivity;
	bool worldSpaceMove;

	bool shootLeftDown;
	bool shootDown;
	bool shootRightDown;
	bool shootLeft;
	bool shootRight;
	bool shootLeftUp;
	bool shootUp;
	bool shootRightUp;
};