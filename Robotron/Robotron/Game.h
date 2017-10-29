//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Controls the gameplay scene and components
//				  
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#define _USE_MATH_DEFINES

// Local Includes
#include "clock.h"
#include "Level.h"

#pragma once
class Game
{
public:
	Game(GLFWwindow* window);
	~Game();

	void executeOneFrame();

	void process(float deltaTick);

private:
	GLFWwindow* m_window;

	Clock m_clock;
	int m_setUpTick = 0;
	Level* m_level;
};