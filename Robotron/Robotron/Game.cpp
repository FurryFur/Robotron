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

GameState Game::s_gameState = MAINMENU;
ButtonState Game::s_buttonState = NOBUTTONDOWN;
bool Game::s_buttonClicked = false;

double Game::s_mousePosX = 0.0f;
double Game::s_mousePosY = 0.0f;
std::vector<IKeyObserver*>  Game::s_keyObservers;

void glfwGetMouseButtonCallBack(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		// If in the main menu screen
		if (Game::s_gameState == MAINMENU)
		{
			if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 450 && Game::s_mousePosY <= 495)
			{
				Game::s_buttonClicked = true;
				Game::s_buttonState = JOINDOWN;
			}

			// The mouse is within the host button click
			else if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 550 && Game::s_mousePosY <= 595)
			{
				Game::s_buttonClicked = true;
				Game::s_buttonState = HOSTDOWN;
			}

			// The mouse is within the quit button click
			else if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 650 && Game::s_mousePosY <= 695)
			{
				Game::s_buttonClicked = true;
				Game::s_buttonState = QUITDOWN;
			}
		}
		// If in the host setup screen
		else if (Game::s_gameState == HOSTSETUP)
		{
			// The mouse is within the back button click
			if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 650 && Game::s_mousePosY <= 695)
			{
				Game::s_buttonClicked = true;
				Game::s_buttonState = BACKDOWN;
			}
		}
		// If in the lobby screen
		else if (Game::s_gameState == LOBBY)
		{
			// The mouse is within the back button click
			if (Game::s_mousePosX >= 135.0f && Game::s_mousePosX <= 280.0f && Game::s_mousePosY >= 650 && Game::s_mousePosY <= 695)
			{
				Game::s_buttonClicked = true;
				Game::s_buttonState = BACKDOWN;
			}
			// The mouse is within the start button click
			if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 650 && Game::s_mousePosY <= 695)
			{
				Game::s_buttonClicked = true;
				Game::s_buttonState = STARTDOWN;
			}
		}
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		// If in the main menu screen
		if (Game::s_gameState == MAINMENU)
		{
			// Join button clicked. Enter search for lobby screen.
			if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 450 && Game::s_mousePosY <= 495 && Game::s_buttonState == JOINDOWN)
			{
				Game::s_gameState = GAME;
			}

			// Host button clicked. Enter host lobby screen.
			else if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 550 && Game::s_mousePosY <= 595 && Game::s_buttonState == HOSTDOWN)
			{
				Game::s_gameState = HOSTSETUP;
			}

			// Quit button clicked. The program is closed.
			else if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 650 && Game::s_mousePosY <= 695 && Game::s_buttonState == QUITDOWN)
			{
				glfwDestroyWindow(window);
				glfwTerminate();
				exit(EXIT_SUCCESS);
			}
		}
		// If in the host setup screen
		else if (Game::s_gameState == HOSTSETUP)
		{
			// The mouse is within the back button click
			if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 650 && Game::s_mousePosY <= 695 && Game::s_buttonState == BACKDOWN)
			{
				Game::s_gameState = MAINMENU;
			}
		}

		// If in the lobby setup screen
		else if (Game::s_gameState == LOBBY)
		{
			// The mouse is within the back button click
			if (Game::s_mousePosX >= 135.0f && Game::s_mousePosX <= 280.0f && Game::s_mousePosY >= 650 && Game::s_mousePosY <= 695 && Game::s_buttonState == BACKDOWN)
			{
				Game::s_gameState = MAINMENU;
			}
			// The mouse is within the start button click
			if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 650 && Game::s_mousePosY <= 695 && Game::s_buttonState == STARTDOWN)
			{
				Game::s_buttonState = NOBUTTONDOWN;
				Game::s_gameState = GAME;
			}
		}
		// The button has been released
		Game::s_buttonState = NOBUTTONDOWN;
	}
}

Game::Game(GLFWwindow* window, Audio audio)
	: m_serverNameInput("", "Assets/Fonts/NYCTALOPIATILT.TTF")
	, m_userNameInput("", "Assets/Fonts/NYCTALOPIATILT.TTF")
	, m_menuScene{}
	, m_renderSystem(window, m_menuScene)
{
	m_clock.Process();
	m_window = window;
	m_displayGameOverText = false;
	m_audio = audio;

	glfwSetMouseButtonCallback(window, &glfwGetMouseButtonCallBack);

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

	//Username input text label;
	m_userNameInput.setPosition(glm::vec2(650.0f, 412.0f));
	m_userNameInput.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	m_userNameInput.setScale(0.5f);

	// Create the main menu join text
	TextLabel join("Join", "Assets/Fonts/NYCTALOPIATILT.TTF");
	join.setPosition(glm::vec2(637.0f, 312.0f));
	join.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiMainMenuLabels.push_back(join);

	// Create the main menu host text
	TextLabel host("Host", "Assets/Fonts/NYCTALOPIATILT.TTF");
	host.setPosition(glm::vec2(632.0f, 212.0f));
	host.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiMainMenuLabels.push_back(host);

	// Create the main menu quit text
	TextLabel quit("Quit", "Assets/Fonts/NYCTALOPIATILT.TTF");
	quit.setPosition(glm::vec2(637.0f, 112.0f));
	quit.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiMainMenuLabels.push_back(quit);

	// Create the main menu controls text
	TextLabel controls("Move: WASD      Shoot: 8 Directions on Numpad", "Assets/Fonts/NYCTALOPIATILT.TTF");
	controls.setScale(0.5f);
	controls.setPosition(glm::vec2(10.0f, 10.0f));
	controls.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiMainMenuLabels.push_back(controls);

	// Create the main menu title text
	TextLabel title("Robotron", "Assets/Fonts/NYCTALOPIATILT.TTF");
	title.setScale(2.0f);
	title.setPosition(glm::vec2(400.0f, 550.0f));
	title.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	m_uiMainMenuLabels.push_back(title);
	
	// Create the main menu sub title text
	TextLabel subtitle("2017", "Assets/Fonts/NYCTALOPIATILT.TTF");
	subtitle.setScale(0.8f);
	subtitle.setPosition(glm::vec2(800.0f, 500.0f));
	subtitle.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	m_uiMainMenuLabels.push_back(subtitle);

	// Create the main menu credits text
	TextLabel credits("Made by: Lance Chaney & Jack Mair", "Assets/Fonts/NYCTALOPIATILT.TTF");
	credits.setScale(0.5f);
	credits.setPosition(glm::vec2(830.0f, 10.0f));
	credits.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiMainMenuLabels.push_back(credits);

	// Create the main menu enter username text
	TextLabel enterUsername("Enter a username: ", "Assets/Fonts/NYCTALOPIATILT.TTF");
	enterUsername.setPosition(glm::vec2(330.0f, 412.0f));
	enterUsername.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	enterUsername.setScale(0.5f);
	m_uiMainMenuLabels.push_back(enterUsername);

	// Create the host setup host text
	TextLabel enter("Enter a server name: ", "Assets/Fonts/NYCTALOPIATILT.TTF");
	enter.setPosition(glm::vec2(100.0f, 312.0f));
	enter.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiHostSetupLabels.push_back(enter);

	// Create the host setup back text
	TextLabel back("Back", "Assets/Fonts/NYCTALOPIATILT.TTF");
	back.setPosition(glm::vec2(637.0f, 112.0f));
	back.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiHostSetupLabels.push_back(back);

	// Create the server name input text label
	m_serverNameInput.setPosition(glm::vec2(100.0f, 212.0f));
	m_serverNameInput.setColor(glm::vec3(0.8f, 0.8f, 0.8f));

	// Create the lobby server name text label
	TextLabel serverName("", "Assets/Fonts/NYCTALOPIATILT.TTF");
	serverName.setPosition(glm::vec2(100.0f, 712.0f));
	serverName.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiLobbyLabels.push_back(serverName);

	// Create the lobby back button text label
	TextLabel lobbyBack("Back", "Assets/Fonts/NYCTALOPIATILT.TTF");
	lobbyBack.setPosition(glm::vec2(137.0f, 112.0f));
	lobbyBack.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiLobbyLabels.push_back(lobbyBack);

	// Create the lobby start game button text label
	TextLabel start("Start!", "Assets/Fonts/NYCTALOPIATILT.TTF");
	start.setPosition(glm::vec2(637.0f, 112.0f));
	start.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiLobbyLabels.push_back(start);

	// Create the game over text label
	TextLabel gameOverMan("Game over man, game over!", "Assets/Fonts/NYCTALOPIATILT.TTF");
	gameOverMan.setScale(0.8f);
	gameOverMan.setPosition(glm::vec2(100.0f, 612.0f));
	gameOverMan.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiGameOverLabels.push_back(gameOverMan);

	// Create the final score text label
	TextLabel finalScore("Final Score: ", "Assets/Fonts/NYCTALOPIATILT.TTF");
	finalScore.setScale(0.8f);
	finalScore.setPosition(glm::vec2(100.0f, 562.0f));
	finalScore.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiGameOverLabels.push_back(finalScore);

}


Game::~Game()
{
}

void Game::registerKeyObserver(IKeyObserver* observer)
{
	s_keyObservers.push_back(observer);
}

// Record Action keys here
void Game::keyCallback(int key, int scancode, int action, int mods)
{
	if (s_gameState != GAME && action == GLFW_PRESS) {
		// Close window on exit
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(m_window, GLFW_TRUE);
			return;
		}

		// Keyboard commands during the Host Setup Screen.
		if (s_gameState == HOSTSETUP)
		{
			// On enter, go to the lobby screen.
			if (key == 257)
			{
				m_uiLobbyLabels.at(0).setText("Server: " + m_serverName);
				s_gameState = LOBBY;
			}
			// Delete characters of the server name if backspace pressed.
			else if (key == 259)
			{
				m_serverName = m_serverName.substr(0, m_serverName.size() - 1);
				m_serverNameInput.setText(m_serverName);
			}
			// Input all other keyboard buttons as characters in the server name.
			else if (key < 255)
			{
				m_serverName += key;
				m_serverNameInput.setText(m_serverName);
			}
		}

		// Keyboard commands during the Host Setup Screen.
		if (s_gameState == MAINMENU)
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
				if(m_userName.size() > 10)
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

void Game::renderMenuScreens()
{
	//glDepthMask(GL_TRUE);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Render the entities in the menu scene
	m_renderSystem.beginRender();

	// Update all the entities using all the systems.
	for (size_t i = 0; i < m_menuScene.getEntityCount(); ++i)
		m_renderSystem.update(m_menuScene.getEntity(i));

	// Render the main menu screen.
	if (s_gameState == MAINMENU)
	{
		for (unsigned int i = 0; i < Game::m_uiMainMenuLabels.size(); ++i)
		{
			m_uiMainMenuLabels.at(i).Render();
			m_userNameInput.Render();
		}

		//m_mousePosLabel.Render();
	}
	// Render the host setup screen.
	else if (s_gameState == HOSTSETUP)
	{
		for (unsigned int i = 0; i < Game::m_uiHostSetupLabels.size(); ++i)
		{
			m_uiHostSetupLabels.at(i).Render();
		}

		m_serverNameInput.Render();
	}
	// Render the host setup screen.
	else if (s_gameState == LOBBY)
	{
		for (unsigned int i = 0; i < Game::m_uiLobbyLabels.size(); ++i)
		{
			m_uiLobbyLabels.at(i).Render();
		}
		// Render the game over labels only after the player has returned from the game
		if (m_displayGameOverText == true)
		{
			for (unsigned int i = 0; i < Game::m_uiGameOverLabels.size(); ++i)
			{
				m_uiGameOverLabels.at(i).Render();
			}
		}
	}

	m_renderSystem.endRender();
	//glfwSwapBuffers(m_window);
}

void Game::process(float deltaTick)
{
	// If not in the game state read the coordinates of the mouse and render the menu
	if (s_gameState != GAME)
	{
		glfwGetCursorPos(m_window, &s_mousePosX, &s_mousePosY);
		renderMenuScreens();
	}
	
	// The Game is currently in the main menu. Here the player can search for a lobby by clicking join or host a lobby.
	if (s_gameState == MAINMENU)
	{
		if (s_buttonClicked == true)
		{
			m_audio.playButtonClick();
			s_buttonClicked = false;
		}
		
		// The mouse is within the join button click
		if (s_mousePosX >= 635.0f && s_mousePosX <= 780.0f && s_mousePosY >= 450.0f && s_mousePosY <= 495.0f)
		{
			if(Game::s_buttonState == NOBUTTONDOWN)
				m_uiMainMenuLabels.at(0).setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			else if (Game::s_buttonState == JOINDOWN)
				m_uiMainMenuLabels.at(0).setColor(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
			m_uiMainMenuLabels.at(0).setColor(glm::vec3(0.8f, 0.8f, 0.8f));

		// The mouse is within the host button click
		if (s_mousePosX >= 635.0f && s_mousePosX <= 780.0f && s_mousePosY >= 550.0f && s_mousePosY <= 595.0f)
		{
			if(Game::s_buttonState == NOBUTTONDOWN)
				m_uiMainMenuLabels.at(1).setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			else if (Game::s_buttonState == HOSTDOWN)
				m_uiMainMenuLabels.at(1).setColor(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
			m_uiMainMenuLabels.at(1).setColor(glm::vec3(0.8f, 0.8f, 0.8f));

		// The mouse is within the quit button click
		if (s_mousePosX >= 635.0f && s_mousePosX <= 780.0f && s_mousePosY >= 650.0f && s_mousePosY <= 695.0f)
		{
			if(Game::s_buttonState == NOBUTTONDOWN)
				m_uiMainMenuLabels.at(2).setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			else if (Game::s_buttonState == QUITDOWN)
				m_uiMainMenuLabels.at(2).setColor(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
			m_uiMainMenuLabels.at(2).setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	}
	// The game is at the search for lobby screen. A list of available lobbies are shown, clicking one allows the player to join it.
	else if (s_gameState == JOINLOBBY)
	{

	}
	// The game is at the host setup screen. Here the player can choose a name of a server before creating it.
	else if (s_gameState == HOSTSETUP)
	{
		// The mouse is within the back button click
		if (s_mousePosX >= 635.0f && s_mousePosX <= 780.0f && s_mousePosY >= 650.0f && s_mousePosY <= 695.0f)
		{
			if (Game::s_buttonState == NOBUTTONDOWN)
				m_uiHostSetupLabels.at(1).setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			else if (Game::s_buttonState == BACKDOWN)
				m_uiHostSetupLabels.at(1).setColor(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
			m_uiHostSetupLabels.at(1).setColor(glm::vec3(0.8f, 0.8f, 0.8f));

	}
	// The game is at the Lobby screen. Players wait here for the game to start. The host can start the game.
	else if (s_gameState == LOBBY)
	{
		// The mouse is within the back button click
		if (s_mousePosX >= 135.0f && s_mousePosX <= 280.0f && s_mousePosY >= 650.0f && s_mousePosY <= 695.0f)
		{
			if (Game::s_buttonState == NOBUTTONDOWN)
				m_uiLobbyLabels.at(1).setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			else if (Game::s_buttonState == BACKDOWN)
				m_uiLobbyLabels.at(1).setColor(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
			m_uiLobbyLabels.at(1).setColor(glm::vec3(0.8f, 0.8f, 0.8f));
		
		// The mouse is within the start button click
		if (s_mousePosX >= 635.0f && s_mousePosX <= 780.0f && s_mousePosY >= 650.0f && s_mousePosY <= 695.0f)
		{
			if (Game::s_buttonState == NOBUTTONDOWN)
				m_uiLobbyLabels.at(2).setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			else if (Game::s_buttonState == STARTDOWN)
				m_uiLobbyLabels.at(2).setColor(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
			m_uiHostSetupLabels.at(1).setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	}
	// The game is in the gameplay screen.
	else if (s_gameState == GAME)
	{
		// Create a level if one does not exist
		if (m_level == nullptr)
			m_level = std::make_unique<Level>(m_window, m_clock, m_audio, m_userName);

		// Process the level
		m_level->process(deltaTick, m_clock);

		// Check if all enemies are dead and spawn new ones if so
		if (!m_level->checkEnemiesAlive())
			m_level->triggerNextLevel();

		// Check if all players are dead and return to lobby if so
		if (!m_level->checkPlayersAlive())
		{
			// Trigger the game over text to dispay and update it
			m_uiGameOverLabels.at(1).setText("Final Score: " + std::to_string(m_level->getPlayerScore()));
			m_displayGameOverText = true;
			// Return to lobby
			s_gameState = LOBBY;

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

	//m_mousePosLabel.setText("X: " + std::to_string(s_mousePosX) + " Y: " + std::to_string(s_mousePosY));

	glfwPollEvents();
}
