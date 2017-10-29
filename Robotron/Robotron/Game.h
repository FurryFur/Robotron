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
#include <string>

enum GameState
{
	MAINMENU,
	JOINLOBBY,
	HOSTSETUP,
	GAME
};

enum ButtonState
{
	NOBUTTONDOWN,
	JOINDOWN,
	HOSTDOWN,
	BACKDOWN,
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
	static std::string s_serverNameInput;

private:

	void registerKeyObserver(IKeyObserver * observer);
	void keyCallback(int key, int scancode, int action, int mods);

	std::vector<IKeyObserver*> m_keyObservers;

	GLFWwindow* m_window;
	Clock m_clock;

	std::vector <TextLabel> m_uiMainMenuText;
	std::vector <TextLabel> m_uiHostSetupText;

	std::unique_ptr<Level> m_level;

	TextLabel m_serverNameInput;

	TextLabel m_mousePosLabel;
};