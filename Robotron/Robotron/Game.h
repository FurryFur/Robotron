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
#include "DummyLevel.h"
#include "TextLabel.h"
#include "Scene.h"
#include "NetworkSystem.h"
#include "NetworkServerSystem.h"
#include "NetworkClientSystem.h"

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
	Game(GLFWwindow* window, Audio audio);
	~Game();

	static GameState s_gameState;
	static ButtonState s_buttonState;
	static double s_mousePosX;
	static double s_mousePosY;
	static size_t numServers; // The total number of successfully reached in broadcast
	static size_t serverNum; // The server number in the vector connected to
	static bool s_buttonClicked;

	static void registerKeyObserver(IKeyObserver * observer);

	static std::vector<IKeyObserver*> s_keyObservers;

	void executeOneFrame();

private:

	void createTextLabel(std::string labelText, glm::vec2 position, std::vector<TextLabel>* screenVector, float scale = 1.0f);
	void renderMenuScreens();
	void process(float deltaTick);
	void keyCallback(int key, int scancode, int action, int mods);

	GLFWwindow* m_window;
	Clock m_clock;

	Scene m_scene;
	Scene m_menuScene;
	RenderSystem m_renderSystem;

	// Each screen state has a vector of text labels that are used to populate the interfaces.
	std::vector <TextLabel> m_uiMainMenuLabels;
	std::vector <TextLabel> m_uiHostSetupLabels;
	std::vector <TextLabel> m_uiLobbyLabels;
	std::vector <TextLabel> m_uiGameOverLabels;
	std::vector <TextLabel> m_uiSearchLobbyLabels; // Each label is repesents an available network to join. Also holds the back button.
	TextLabel m_SearchLobbyBackButton;

	std::unique_ptr<NetworkSystem> m_networkSystem;

	std::unique_ptr<Level> m_level;
	std::unique_ptr<DummyLevel> m_dummyLevel;
	
	Audio m_audio; // the audio device
	std::string m_serverName; // the name of the server
	TextLabel m_serverNameInput; // displays the servername
	std::string m_userName; // the username of the player
	TextLabel m_userNameInput; //displays the suervername
	bool m_displayGameOverText; // flags wether the player reach the menu screen via a game over
	bool m_isHost; // A flag to tell the system that the player is running the game as the host
	bool m_checkLoss; // A flag to check loss. Cliennt only checks for loss after they have connected properly
	//TextLabel m_mousePosLabel;
};