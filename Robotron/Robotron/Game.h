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
	LOBBY,
	GAME
};

enum ButtonState
{
	NOBUTTONDOWN,
	JOINDOWN,
	HOSTDOWN,
	BACKDOWN,
	STARTDOWN,
	QUITDOWN
};

#pragma once
class Game
{
public:
	Game(GLFWwindow* window);
	~Game();

	void executeOneFrame();

	void renderMenuScreens();

	void process(float deltaTick);

	static GameState s_gameState;
	static ButtonState s_buttonState;
	static double s_mousePosX;
	static double s_mousePosY;

	static void registerKeyObserver(IKeyObserver * observer);

	static std::vector<IKeyObserver*> s_keyObservers;

private:

	void keyCallback(int key, int scancode, int action, int mods);

	GLFWwindow* m_window;
	Clock m_clock;

	// Each screen state has a vector of text labels that are used to populate the interfaces.
	std::vector <TextLabel> m_uiMainMenuLabels;
	std::vector <TextLabel> m_uiHostSetupLabels;
	std::vector <TextLabel> m_uiLobbyLabels;
	std::vector <TextLabel> m_uiGameOverLabels;

	std::unique_ptr<Level> m_level;
	
	std::string m_serverName;
	TextLabel m_serverNameInput;

	bool m_displayGameOverText;

	//TextLabel m_mousePosLabel;
};