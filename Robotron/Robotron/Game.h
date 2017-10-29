//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Controls the clock and flow between different screen states
//				  
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#define _USE_MATH_DEFINES

// Local Includes
#include "clock.h"
#include "Level.h"
#include "TextLabel.h"

#include <memory>

enum GameState
{
	MAINMENU,
	LOBBY,
	GAME
};

enum ButtonState
{
	NOBUTTONDOWN,
	JOINDOWN,
	HOSTDOWN,
	QUITDOWN
};

#pragma once
class Game
{
public:
	Game(GLFWwindow* window);
	~Game();

	void executeOneFrame();

	void render();

	void process(float deltaTick);

	static GameState s_gameState;
	static ButtonState s_buttonState;
	static double s_mousePosX;
	static double s_mousePosY;

private:

	GLFWwindow* m_window;
	Clock m_clock;

	std::unique_ptr<Level> m_level;

	std::vector <TextLabel> m_uiText;

	TextLabel m_mousePosLabel;
};