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

#include "Game.h"

#include "RenderSystem.h"
#include "Audio.h"

#include <cstdint>

std::vector<IKeyObserver*>  Game::s_keyObservers;

Game::Game(GLFWwindow* window, Audio audio)
	: m_scene{}
	, m_serverNameInput("", "Assets/Fonts/NYCTALOPIATILT.TTF")
	, m_userNameInput("", "Assets/Fonts/NYCTALOPIATILT.TTF")
	, m_menuScene{}
	, m_renderSystem(window, m_menuScene)
	, m_BackButton("Back", "Assets/Fonts/NYCTALOPIATILT.TTF")
{
	m_clock.Process();
	m_window = window;
	m_audio = audio;
	m_gameState = MAINMENU;
	m_buttonState = NOBUTTONDOWN;

	// Create the skybox
	Entity& skybox = EntityUtils::createSkybox(m_menuScene, {
		"Assets/Textures/envmap_violentdays/violentdays_rt.tga",
		"Assets/Textures/envmap_violentdays/violentdays_lf.tga",
		"Assets/Textures/envmap_violentdays/violentdays_up.tga",
		"Assets/Textures/envmap_violentdays/violentdays_dn.tga",
		"Assets/Textures/envmap_violentdays/violentdays_bk.tga",
		"Assets/Textures/envmap_violentdays/violentdays_ft.tga",
	});

	Texture irradianceMap = GLUtils::loadCubeMapFaces({
		"Assets/Textures/envmap_violentdays/violentdays_irr_c00.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c01.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c02.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c03.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c04.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c05.bmp",
	});

	Texture radianceMap = GLUtils::loadDDSTexture("Assets/Textures/envmap_violentdays/violentdays_pmrem.dds");

	// Set skybox as environment map for reflections
	// The skybox only has one colormap texture so use this as the reflection map.
	m_renderSystem.setRadianceMap(radianceMap.id);
	m_renderSystem.setIrradianceMap(irradianceMap.id);

	// Setup the camera
	Entity& cameraEntity = EntityUtils::createCamera(m_menuScene, { 0, 40, 20 }, { 0, 0, 0 }, { 0, 1, 0 });
	m_renderSystem.setCamera(&cameraEntity);

	// Register input system as a listener for keyboard events
	glfwSetWindowUserPointer(window, this);
	auto keyFunc = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
		game->keyCallback(key, scancode, action, mods);
	};
	glfwSetKeyCallback(window, keyFunc);

	// Register input system as a listener for mouse events
	auto mouseFunc = [](GLFWwindow* window, int button, int action, int mods) {
		Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
		game->mouseButtonCallBack(window, button, action, mods);
	};
	glfwSetMouseButtonCallback(window, mouseFunc);

	//Username input text label;
	m_userNameInput.setPosition(glm::vec2(650.0f, 412.0f));
	m_userNameInput.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	m_userNameInput.setScale(0.5f);

	// Create the main menu join text
	createTextLabel("Join", glm::vec2(637.0f, 312.0f), &m_uiMainMenuLabels);
	// Create the main menu host text
	createTextLabel("Host", glm::vec2(632.0f, 212.0f), &m_uiMainMenuLabels);
	// Create the main menu quit text
	createTextLabel("Quit", glm::vec2(637.0f, 112.0f), &m_uiMainMenuLabels);
	// Create the main menu controls text
	createTextLabel("Move: WASD      Shoot: 8 Directions on Numpad", glm::vec2(10.0f, 10.0f), &m_uiMainMenuLabels, 0.5f);
	// Create the main menu title text
	createTextLabel("Doge-otron", glm::vec2(350.0f, 550.0f), &m_uiMainMenuLabels, 2.0f);
	// Create the main menu sub title text
	createTextLabel("2017", glm::vec2(800.0f, 500.0f), &m_uiMainMenuLabels, 0.8f);
	// Create the main menu credits text
	createTextLabel("Made by: Lance Chaney & Jack Mair", glm::vec2(830.0f, 10.0f), &m_uiMainMenuLabels, 0.5f);
	// Create the main menu enter username text
	createTextLabel("Enter a username: ", glm::vec2(330.0f, 412.0f), &m_uiMainMenuLabels, 0.5f);

	// Create the host setup host text
	createTextLabel("Enter a server name: ", glm::vec2(100.0f, 312.0f), &m_uiHostSetupLabels);
	// Create the host setup back text
	createTextLabel("Back", glm::vec2(137.0f, 112.0f), &m_uiHostSetupLabels);

	// Create the server name input text label
	m_serverNameInput.setPosition(glm::vec2(100.0f, 212.0f));
	m_serverNameInput.setColor(glm::vec3(0.8f, 0.8f, 0.8f));

	// Create the search lobby back text
	m_BackButton.setPosition(glm::vec2(137.0f, 112.0f));
	m_BackButton.setColor(glm::vec3(0.8f, 0.8f, 0.8f));

	// Create the lobby server name text label
	createTextLabel("", glm::vec2(100.0f, 712.0f), &m_uiLobbyLabels);
	// Create the lobby back button text label
	createTextLabel("Back", glm::vec2(137.0f, 112.0f), &m_uiLobbyLabels);
	// Create the lobby start game button text label
	createTextLabel("Start!", glm::vec2(637.0f, 112.0f), &m_uiLobbyLabels);
	
	// Create the game over text label
	createTextLabel("Game over man, game over!", glm::vec2(100.0f, 612.0f), &m_uiGameOverLabels);
}


Game::~Game()
{
}

void Game::mouseButtonCallBack(GLFWwindow* window, int button, int action, int mods)
{
	// The left mouse has been pressed
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		switch (m_gameState)
		{
		case MAINMENU:
		{
			if (m_mousePosX >= 635.0f && m_mousePosX <= 780.0f && m_mousePosY >= 450 && m_mousePosY <= 495)
			{
				m_buttonClicked = true;
				m_buttonState = JOINDOWN;
			}

			// The mouse is within the host button click
			else if (m_mousePosX >= 635.0f && m_mousePosX <= 780.0f && m_mousePosY >= 550 && m_mousePosY <= 595)
			{
				m_buttonClicked = true;
				m_buttonState = HOSTDOWN;
			}

			// The mouse is within the quit button click
			else if (m_mousePosX >= 635.0f && m_mousePosX <= 780.0f && m_mousePosY >= 650 && m_mousePosY <= 695)
			{
				m_buttonClicked = true;
				m_buttonState = QUITDOWN;
			}
			break;
		}
		case HOSTSETUP:
		{
			// The mouse is within the back button click
			if (m_mousePosX >= 135.0f && m_mousePosX <= 280.0f && m_mousePosY >= 650 && m_mousePosY <= 695)
			{
				m_buttonClicked = true;
				m_buttonState = BACKDOWN;
			}
			break;
		}
		case LOBBY:
		{
			// The mouse is within the back button click
			if (m_mousePosX >= 135.0f && m_mousePosX <= 280.0f && m_mousePosY >= 650 && m_mousePosY <= 695)
			{
				m_buttonClicked = true;
				m_buttonState = BACKDOWN;
			}
			// The mouse is within the start button click
			if (m_mousePosX >= 635.0f && m_mousePosX <= 780.0f && m_mousePosY >= 650 && m_mousePosY <= 695)
			{
				m_buttonClicked = true;
				m_buttonState = STARTDOWN;
			}
			break;
		}
		case CLIENTLOBBY:
		{
			// The mouse is within the back button click
			if (m_mousePosX >= 135.0f && m_mousePosX <= 280.0f && m_mousePosY >= 650 && m_mousePosY <= 695)
			{
				m_buttonClicked = true;
				m_buttonState = BACKDOWN;
			}
			break;
		}
		case GAMEOVER:
		{
			// The mouse is within the back button click
			if (m_mousePosX >= 135.0f && m_mousePosX <= 280.0f && m_mousePosY >= 650 && m_mousePosY <= 695)
			{
				m_buttonClicked = true;
				m_buttonState = BACKDOWN;
			}
			break;
		}
		case LOBBYSEARCH:
		{
			// The mouse is within the back button click
			if (m_mousePosX >= 135.0f && m_mousePosX <= 280.0f && m_mousePosY >= 650 && m_mousePosY <= 695)
			{
				m_buttonClicked = true;
				m_buttonState = BACKDOWN;
			}
			// Handlers the visual interaction of the server buttons.
			for (size_t i = 0; i < m_numServers; ++i)
			{
				// The mouse is within the join button click
				if (m_mousePosY >= 23.0f + i * 40.0f && m_mousePosY <= 63.0f + i * 40.0f)
				{
					m_uiLobbyLabels.at(0).setText("Server: " + m_serverNames.at(i));
					static_cast<NetworkClientSystem*>(m_networkSystem.get())->joinServer(m_serverAddresses.at(i));
				}
			}
		}
		default:
			break;
		}

	}
	// The left mouse has been released.
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		switch (m_gameState)
		{
		case MAINMENU:
		{
			// Join button clicked. Enter search for lobby screen.
			if (m_mousePosX >= 635.0f && m_mousePosX <= 780.0f && m_mousePosY >= 450 && m_mousePosY <= 495 && m_buttonState == JOINDOWN)
			{
				m_gameState = LOBBYSEARCH;
				m_uiSearchLobbyLabels.clear();
				m_numServers = 0;
				m_serverNames.clear();
				m_serverAddresses.clear();
			}

			// Host button clicked. Enter host lobby screen.
			else if (m_mousePosX >= 635.0f && m_mousePosX <= 780.0f && m_mousePosY >= 550 && m_mousePosY <= 595 && m_buttonState == HOSTDOWN)
			{
				m_gameState = HOSTSETUP;
			}

			// Quit button clicked. The program is closed.
			else if (m_mousePosX >= 635.0f && m_mousePosX <= 780.0f && m_mousePosY >= 650 && m_mousePosY <= 695 && m_buttonState == QUITDOWN)
			{
				glfwDestroyWindow(window);
				glfwTerminate();
				exit(EXIT_SUCCESS);
			}
			break;
		}
		case HOSTSETUP:
		{
			// The mouse is within the back button click
			if (m_mousePosX >= 135.0f && m_mousePosX <= 280.0f && m_mousePosY >= 650 && m_mousePosY <= 695 && m_buttonState == BACKDOWN)
			{
				m_resetGame = true;
			}
			break;
		}
		case LOBBYSEARCH:
		{
			// The mouse is within the back button click
			if (m_mousePosX >= 135.0f && m_mousePosX <= 280.0f && m_mousePosY >= 650 && m_mousePosY <= 695 && m_buttonState == BACKDOWN)
			{
				m_resetGame = true;
			}
			break;
		}
		case GAMEOVER:
		{
			// The mouse is within the back button click
			if (m_mousePosX >= 135.0f && m_mousePosX <= 280.0f && m_mousePosY >= 650 && m_mousePosY <= 695 && m_buttonState == BACKDOWN)
			{
				m_resetGame = true;
			}
			break;
		}
		case LOBBY:
		{
			// The mouse is within the back button click
			if (m_mousePosX >= 135.0f && m_mousePosX <= 280.0f && m_mousePosY >= 650 && m_mousePosY <= 695 && m_buttonState == BACKDOWN)
			{
				m_resetGame = true;
			}
			// The mouse is within the start button click
			if (m_mousePosX >= 635.0f && m_mousePosX <= 780.0f && m_mousePosY >= 650 && m_mousePosY <= 695 && m_buttonState == STARTDOWN)
			{
				m_buttonState = NOBUTTONDOWN;
				m_gameState = GAME;
				m_networkSystem->startGame();
			}
			break;
		}
		case CLIENTLOBBY:
		{
			// The mouse is within the back button click
			if (m_mousePosX >= 135.0f && m_mousePosX <= 280.0f && m_mousePosY >= 650 && m_mousePosY <= 695 && m_buttonState == BACKDOWN)
			{
				m_resetGame = true;
			}
			break;
		}
		default:
		{
			break;
		}

		}
		// Reset the buttonstate
		m_buttonState = NOBUTTONDOWN;
	}
}

void Game::createTextLabel(std::string labelText, glm::vec2 position, std::vector<TextLabel>* screen, float scale)
{
	TextLabel label(labelText, "Assets/Fonts/NYCTALOPIATILT.TTF");
	label.setScale(scale);
	label.setPosition(position);
	label.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	screen->push_back(label);
}

void Game::checkButtonHighlight(float minX, float maxX, float minY, float maxY, TextLabel* button, ButtonState buttonType)
{
	if (m_mousePosX >= minX && m_mousePosX <= maxX && m_mousePosY >= minY && m_mousePosY <= maxY)
	{
		if (m_buttonState == NOBUTTONDOWN)
			button->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		else if (m_buttonState == buttonType)
			button->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else
		button->setColor(glm::vec3(0.8f, 0.8f, 0.8f));
}

void Game::registerKeyObserver(IKeyObserver* observer)
{
	s_keyObservers.push_back(observer);
}

// Record Action keys here
void Game::keyCallback(int key, int scancode, int action, int mods)
{
	if (m_gameState != GAME && action == GLFW_PRESS) {
		// Close window on exit
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(m_window, GLFW_TRUE);
			return;
		}

		// Keyboard commands during the Host Setup Screen.
		if (m_gameState == HOSTSETUP)
		{
			// On enter, go to the lobby screen.
			if (key == GLFW_KEY_ENTER)
			{
				m_uiLobbyLabels.at(0).setText("Server: " + m_serverName);
				m_gameState = LOBBY;
				m_isHost = true;
			}
			// Delete characters of the server name if backspace pressed.
			else if (key == GLFW_KEY_BACKSPACE)
			{
				m_serverName = m_serverName.substr(0, m_serverName.size() - 1);
				m_serverNameInput.setText(m_serverName);
			}
			// Input all other keyboard buttons as characters in the server name.
			else if (key <= 90 && key >= 48)
			{
				m_serverName += key;
				m_serverNameInput.setText(m_serverName);
			}
		}

		// Keyboard commands during the Host Setup Screen.
		if (m_gameState == MAINMENU)
		{
			// Delete characters of the server name if backspace pressed.
			if (key == 259)
			{
				m_userName = m_userName.substr(0, m_userName.size() - 1);
				m_userNameInput.setText(m_userName);
			}
			// Input all other keyboard buttons as characters in the server name.
			else if (key < 255)
			{
				m_userName += key;
				// Limit the size of the username to 10 characters
				if (m_userName.size() > 10)
					m_userName = m_userName.substr(0, m_userName.size() - 1);
				m_userNameInput.setText(m_userName);
			}
		}
	}

	for (auto& observer : s_keyObservers)
		observer->keyCallback(key, scancode, action, mods);
}

void Game::executeOneFrame()
{
	m_clock.Process();
	float fDT = m_clock.GetDeltaTick();
	process(fDT);
}

bool Game::checkReset()
{
	return m_resetGame;
}

void Game::renderMenuScreens()
{
	m_renderSystem.beginRender();

	//Update the render system for the entities in the scene
	for (size_t i = 0; i < m_menuScene.getEntityCount(); ++i)
		m_renderSystem.update(m_menuScene.getEntity(i));

	// Render the menu UIs.
	switch(m_gameState)
	{
	case MAINMENU:
	{
		for (unsigned int i = 0; i < m_uiMainMenuLabels.size(); ++i)
		{
			m_uiMainMenuLabels.at(i).Render();
			m_userNameInput.Render();
		}
		break;
	}
	case GAMEOVER:
	{
		for (unsigned int i = 0; i < m_uiGameOverLabels.size(); ++i)
		{
			m_uiGameOverLabels.at(i).Render();
		}
		for (unsigned int i = 0; i < m_uiPlayerNames.size(); ++i)
		{
			m_uiPlayerNames.at(i).Render();
		}
		m_BackButton.Render();
		break;
	}
	case LOBBYSEARCH:
	{
		m_BackButton.Render();
		for (unsigned int i = 0; i < m_uiSearchLobbyLabels.size(); ++i)
		{
			m_uiSearchLobbyLabels.at(i).Render();
		}
		break;
	}
	case HOSTSETUP:
	{
		for (unsigned int i = 0; i < m_uiHostSetupLabels.size(); ++i)
		{
			m_uiHostSetupLabels.at(i).Render();
		}

		m_serverNameInput.Render();
		break;
	}
	case LOBBY:
	{
		// Display the UI
		for (unsigned int i = 0; i < m_uiLobbyLabels.size(); ++i)
		{
			m_uiLobbyLabels.at(i).Render();
		}
		// Display all the player names
		for (unsigned int i = 0; i < m_uiPlayerNames.size(); ++i)
		{
			m_uiPlayerNames.at(i).Render();
		}
		break;
	}
	case CLIENTLOBBY:
	{
		m_uiLobbyLabels.at(0).Render(); // Render the server name
		m_uiLobbyLabels.at(1).Render(); // Redner the back button
		// Display all the player names
		for (unsigned int i = 0; i < m_uiPlayerNames.size(); ++i)
		{
			m_uiPlayerNames.at(i).Render();
		}
		break;
	}
	default:
	{
		break;
	}
	}

	//m_mousePosLabel.Render();
	m_renderSystem.endRender();
}

void Game::process(float deltaTick)
{

	if (m_networkSystem)
		m_networkSystem->beginFrame();

	// If not in the game state read the coordinates of the mouse and render the menu
	if (m_gameState != GAME)
	{
		glfwGetCursorPos(m_window, &m_mousePosX, &m_mousePosY);
		renderMenuScreens();
	}
	
	// The Game is currently in the main menu. Here the player can search for a lobby by clicking join or host a lobby.
	switch(m_gameState)
	{
	case MAINMENU:
	{
		// Reset the network
		if (m_networkSystem != nullptr)
			m_networkSystem.reset(nullptr);

		// Reset the player's ID num
		if(m_setPlayerIDNum)
			m_setPlayerIDNum = false;

		//Set the host to be false if the player is in the main menu
		if (m_isHost)
			m_isHost = false;

		if (m_buttonClicked == true)
		{
			m_audio.playButtonClick();
			m_buttonClicked = false;
		}

		// The mouse is within the join button click
		checkButtonHighlight(635.0f, 780.0f, 450.0f, 495.0f, &m_uiMainMenuLabels.at(0), JOINDOWN);
		// The mouse is within the host button click
		checkButtonHighlight(635.0f, 780.0f, 550.0f, 595.0f, &m_uiMainMenuLabels.at(1), HOSTDOWN);
		// The mouse is within the quit button click
		checkButtonHighlight(635.0f, 780.0f, 650.0f, 695.0f, &m_uiMainMenuLabels.at(2), QUITDOWN);
		break;
	}
	// The game is at the search for lobby screen. A list of available lobbies are shown, clicking one allows the player to join it.
	case LOBBYSEARCH:
	{
		static NetworkClientSystem* s_client;
		// If you are the host create the network system
		if (m_networkSystem == nullptr)
		{
			std::unique_ptr<NetworkClientSystem> temp = std::make_unique<NetworkClientSystem>(m_scene, m_audio, m_userName);
			s_client = temp.get();
			m_networkSystem = std::move(temp);
			s_client->registerLobbyEventListener(this);
			s_client->broadcastForServers();
		}
		// If he number of servers no longer = the number of buttons. Reset the buttons
		if (m_numServers != m_uiSearchLobbyLabels.size())
		{
			m_uiSearchLobbyLabels.clear();
			for (size_t i = 0; i < m_numServers; ++i)
			{
				if (m_serverNames.size() >= i)
					createTextLabel(m_serverNames.at(i), glm::vec2(137.0f, 750.0f - i * 40), &m_uiSearchLobbyLabels, 0.5f);
				else
					std::cout << "Error: Tried to create a label for a server that does not exist";
			}
		}

		// Handlers the visual interaction of the server buttons.
		for (size_t i = 0; i < m_numServers; ++i)
		{
			// The mouse is within the join button click
			checkButtonHighlight(0, 800.0f, 23.0f + i * 40.0f, 63.0f + i * 40.0f, &m_uiSearchLobbyLabels.at(i), JOINDOWN);
		}

		// The mouse is within the back button click
		checkButtonHighlight(135.0f, 280.0f, 650.0f, 695.0f, &m_BackButton, BACKDOWN);
		break;
	}
	case HOSTSETUP:
	{
		//Set the ID num equal to zero as you are the host
		if (!m_setPlayerIDNum)
		{
			m_playerID = 0;
			m_setPlayerIDNum = true;
		}
		
		// The mouse is within the back button click
		checkButtonHighlight(135.0f, 280.0f, 650.0f, 695.0f, &m_uiHostSetupLabels.at(1), BACKDOWN);
		break;
	}
	case CLIENTLOBBY:
	{
		// The mouse is within the back button click
		checkButtonHighlight(135.0f, 280.0f, 650.0f, 695.0f, &m_uiLobbyLabels.at(1), BACKDOWN);
	}
	case LOBBY:
	{
		// If you are the host create the network system
		if (m_networkSystem == nullptr) {
			auto tmp = std::make_unique<NetworkServerSystem>(m_scene, m_userName, m_serverName);
			m_audio.setNetworkAudioServer(tmp.get());
			m_networkSystem = std::move(tmp);
		}
		m_networkSystem->registerLobbyEventListener(this);

		// The mouse is within the back button click
		checkButtonHighlight(135.0f, 280.0f, 650.0f, 695.0f, &m_uiLobbyLabels.at(1), BACKDOWN);

		// The mouse is within the start button click
		checkButtonHighlight(635.0f, 780.0f, 650.0f, 695.0f, &m_uiLobbyLabels.at(2), STARTDOWN);
		break;
	}
	case GAME:
	{
		if (m_isHost)
		{			
			// Create a level if one does not exist
			if (m_level == nullptr)
				m_level = std::make_unique<Level>(m_window, m_clock, m_audio, m_scene, m_userName, m_playerID, *m_networkSystem);

			// Process the level
			m_level->process(deltaTick, m_clock);

			// Check if all enemies are dead and spawn new ones if so
			if (!m_level->checkEnemiesAlive())
				m_level->triggerNextLevel();

			// Check if all players are dead and return to lobby if so
			if (!m_level->checkPlayersAlive())
			{
				// Return to main menu
				m_gameState = GAMEOVER;

				// Register input system as a listener for keyboard events
				glfwSetWindowUserPointer(m_window, this);
				auto keyFunc = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
					Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
					game->keyCallback(key, scancode, action, mods);
				};
				glfwSetKeyCallback(m_window, keyFunc);

				// Reset the level
				m_level.reset(nullptr);
			}
		}
		else
		{
			// Create a level if one does not exist
			if (m_dummyLevel == nullptr)
				m_dummyLevel = std::make_unique<DummyLevel>(m_window, m_clock, m_scene, m_userName, m_playerID, *m_networkSystem);

			// Process the level
			m_dummyLevel->process(deltaTick, m_clock);

			// Check if the game has started and the client can start checking for loss
			if (!m_checkLoss)
			{
				for (unsigned int i = 0; i < m_scene.getEntityCount(); ++i)
				{
					if (m_scene.getEntity(i).hasComponents(COMPONENT_PLAYER) && m_scene.getEntity(i).player.playerInfo.getLives() > 0)
						m_checkLoss = true;
				}
			}

			// Check if all players are dead and return to lobby if so
			if (m_checkLoss && !m_dummyLevel->checkPlayersAlive())
			{
				// Return to main menu
				m_gameState = GAMEOVER;

				// Register input system as a listener for keyboard events
				glfwSetWindowUserPointer(m_window, this);
				auto keyFunc = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
					Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
					game->keyCallback(key, scancode, action, mods);
				};
				glfwSetKeyCallback(m_window, keyFunc);

				// Reset the level
				m_dummyLevel.reset(nullptr);
			}
		}
		break;
	}
	default:
		break;
	}

	if (m_networkSystem)
		m_networkSystem->endFrame();

	glfwPollEvents();
}

void Game::onBroadcastResponse(const std::string& serverName, const sockaddr_in& serverAddress)
{
	++m_numServers;
	m_serverNames.push_back(serverName);
	m_serverAddresses.push_back(serverAddress);
}

void Game::onJoinAccepted()
{
	m_gameState = CLIENTLOBBY;
}

void Game::onJoinRejected()
{
	std::cout << "Error: Failed to connect to the server.";
}

void Game::onPlayersUpdated(const std::vector<PlayerInfo>& playerList)
{
	//Set the ID num equal to the number of connected players
	if (!m_setPlayerIDNum)
	{
		m_playerID = static_cast<std::uint8_t>(playerList.size() - 1);
		m_setPlayerIDNum = true;
	}
	
	//Check if m_numofConnectPlayers is different to the number connected
	if (m_numConnectedPlayers != playerList.size())
	{
		m_uiPlayerNames.clear();
		for (size_t i = 0; i < playerList.size(); ++i)
		{
			if (m_uiPlayerNames.size() >= i)
			{
				if (playerList.at(i).getScore() != 0)
				{
					std::string temp = playerList.at(i).username + ": " + std::to_string(playerList.at(i).getScore());
					createTextLabel(temp, glm::vec2(180.0f, 550.0f - i * 40), &m_uiPlayerNames, 0.5f);
				}
				else 
					createTextLabel(playerList.at(i).username, glm::vec2(180.0f, 550.0f - i * 40), &m_uiPlayerNames, 0.5f);
			}
			else
				std::cout << "Error: Tried to create a label for a player that does not exist";
		}

		m_numConnectedPlayers = playerList.size();
	}
}

void Game::onGameStart()
{
	m_gameState = GAME;
}

void Game::onDisconnect()
{
	m_resetGame = true;
}
