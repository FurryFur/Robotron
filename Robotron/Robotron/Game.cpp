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
std::string Game::s_serverNameInput = "";

double Game::s_mousePosX = 0.0f;
double Game::s_mousePosY = 0.0f;


void glfwGetMouseButtonCallBack(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		// If in the main menu screen
		if (Game::s_gameState == MAINMENU)
		{
			if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 450 && Game::s_mousePosY <= 495)
			{
				Game::s_buttonState = JOINDOWN;
			}

			// The mouse is within the host button click
			else if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 550 && Game::s_mousePosY <= 595)
			{
				Game::s_buttonState = HOSTDOWN;
			}

			// The mouse is within the quit button click
			else if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 650 && Game::s_mousePosY <= 695)
			{
				Game::s_buttonState = QUITDOWN;
			}
		}
		// If in the host setup screen
		else if (Game::s_gameState == HOSTSETUP)
		{
			// The mouse is within the back button click
			if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 650 && Game::s_mousePosY <= 695)
			{
				Game::s_buttonState = BACKDOWN;
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

		// The button has been released
		Game::s_buttonState = NOBUTTONDOWN;
	}
}

Game::Game(GLFWwindow* window)
	: m_mousePosLabel("Basic Game", "Assets/Fonts/NYCTALOPIATILT.TTF")
	, m_serverNameInput("", "Assets/Fonts/NYCTALOPIATILT.TTF")
{
	m_clock.Process();
	m_window = window;

	glfwSetMouseButtonCallback(window, &glfwGetMouseButtonCallBack);

	m_mousePosLabel.setPosition(glm::vec2(10.0f, 10.0f));
	m_mousePosLabel.setColor(glm::vec3(0.8f, 0.8F, 0.8f));

	// Register input system as a listener for keyboard events
	glfwSetWindowUserPointer(window, this);
	auto keyFunc = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		Game* inputSystem = static_cast<Game*>(glfwGetWindowUserPointer(window));
		inputSystem->keyCallback(key, scancode, action, mods);
	};
	glfwSetKeyCallback(window, keyFunc);

	// Create the main menu join text
	TextLabel join("Join", "Assets/Fonts/NYCTALOPIATILT.TTF");
	join.setPosition(glm::vec2(637.0f, 312.0f));
	join.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiMainMenuText.push_back(join);

	// Create the main menu host text
	TextLabel host("Host", "Assets/Fonts/NYCTALOPIATILT.TTF");
	host.setPosition(glm::vec2(632.0f, 212.0f));
	host.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiMainMenuText.push_back(host);

	// Create the main menu quit text
	TextLabel quit("Quit", "Assets/Fonts/NYCTALOPIATILT.TTF");
	quit.setPosition(glm::vec2(637.0f, 112.0f));
	quit.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiMainMenuText.push_back(quit);

	// Create the host setup host text
	TextLabel enter("Enter a server name: ", "Assets/Fonts/NYCTALOPIATILT.TTF");
	enter.setPosition(glm::vec2(100.0f, 312.0f));
	enter.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiHostSetupText.push_back(enter);

	// Create the host setup back text
	TextLabel back("Back", "Assets/Fonts/NYCTALOPIATILT.TTF");
	back.setPosition(glm::vec2(637.0f, 112.0f));
	back.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	m_uiHostSetupText.push_back(back);

	// Create the server name input text label
	m_serverNameInput.setPosition(glm::vec2(100.0f, 212.0f));
	m_serverNameInput.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
}


Game::~Game()
{
}

void Game::registerKeyObserver(IKeyObserver* observer)
{
	m_keyObservers.push_back(observer);
}

// Record Action keys here
void Game::keyCallback(int key, int scancode, int action, int mods)
{
	if (s_gameState != GAME) {
		// Close window on exit
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(m_window, GLFW_TRUE);
			return;
		}

		for (auto& observer : m_keyObservers)
			observer->keyCallback(key, scancode, action, mods);
	}
}


void Game::executeOneFrame()
{
	m_clock.Process();
	float fDT = m_clock.GetDeltaTick();
	process(fDT);
}

void Game::render()
{
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render all the labels on screen dependant on menu state;
	if (s_gameState == MAINMENU)
	{
		for (unsigned int i = 0; i < Game::m_uiMainMenuText.size(); ++i)
		{
			m_uiMainMenuText.at(i).Render();
		}

		m_mousePosLabel.Render();
	}
	if (s_gameState == HOSTSETUP)
	{
		for (unsigned int i = 0; i < Game::m_uiHostSetupText.size(); ++i)
		{
			m_uiHostSetupText.at(i).Render();
		}

		m_serverNameInput.Render();
	}
	glfwSwapBuffers(m_window);
}

void Game::process(float deltaTick)
{
	// If not in the game state read the coordinates of the mouse
	if(s_gameState!= GAME)	
		glfwGetCursorPos(m_window, &s_mousePosX, &s_mousePosY);
	
	// The Game is currently in the main menu.
	if (s_gameState == MAINMENU)
	{
		// The mouse is within the join button click
		if (s_mousePosX >= 635.0f && s_mousePosX <= 780.0f && s_mousePosY >= 450.0f && s_mousePosY <= 495.0f)
		{
			if(Game::s_buttonState == NOBUTTONDOWN)
				m_uiMainMenuText.at(0).setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			else if (Game::s_buttonState == JOINDOWN)
				m_uiMainMenuText.at(0).setColor(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
			m_uiMainMenuText.at(0).setColor(glm::vec3(0.8f, 0.8f, 0.8f));

		// The mouse is within the host button click
		if (s_mousePosX >= 635.0f && s_mousePosX <= 780.0f && s_mousePosY >= 550.0f && s_mousePosY <= 595.0f)
		{
			if(Game::s_buttonState == NOBUTTONDOWN)
				m_uiMainMenuText.at(1).setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			else if (Game::s_buttonState == HOSTDOWN)
				m_uiMainMenuText.at(1).setColor(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
			m_uiMainMenuText.at(1).setColor(glm::vec3(0.8f, 0.8f, 0.8f));

		// The mouse is within the quit button click
		if (s_mousePosX >= 635.0f && s_mousePosX <= 780.0f && s_mousePosY >= 650.0f && s_mousePosY <= 695.0f)
		{
			if(Game::s_buttonState == NOBUTTONDOWN)
				m_uiMainMenuText.at(2).setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			else if (Game::s_buttonState == QUITDOWN)
				m_uiMainMenuText.at(2).setColor(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
			m_uiMainMenuText.at(2).setColor(glm::vec3(0.8f, 0.8f, 0.8f));

		render();
	}
	// The game is at the Lobby screen.
	else if (s_gameState == JOINLOBBY)
	{

	}
	// The game is at the Lobby screen.
	else if (s_gameState == HOSTSETUP)
	{
		m_serverNameInput.setText(s_serverNameInput);
		// The mouse is within the quit button click
		if (s_mousePosX >= 635.0f && s_mousePosX <= 780.0f && s_mousePosY >= 650.0f && s_mousePosY <= 695.0f)
		{
			if (Game::s_buttonState == NOBUTTONDOWN)
				m_uiHostSetupText.at(1).setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			else if (Game::s_buttonState == BACKDOWN)
				m_uiHostSetupText.at(1).setColor(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
			m_uiHostSetupText.at(1).setColor(glm::vec3(0.8f, 0.8f, 0.8f));

		render();
	}
	// The game is in the gameplay screen.
	else if (s_gameState == GAME)
	{
		if (m_level == NULL)
			m_level = std::make_unique<Level>(m_window);

		m_level->process(deltaTick, m_clock);

		if (!m_level->checkEnemiesAlive())
			m_level->triggerNextLevel();
	}

	m_mousePosLabel.setText("X: " + std::to_string(s_mousePosX) + " Y: " + std::to_string(s_mousePosY));

	glfwPollEvents();
}
