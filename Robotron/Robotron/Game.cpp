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

double Game::s_mousePosX = 0.0f;
double Game::s_mousePosY = 0.0f;

void glfwGetMouseButtonCallBack(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
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
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 450 && Game::s_mousePosY <= 495 && Game::s_buttonState == JOINDOWN)
		{
			Game::s_gameState = GAME;
		}

		// The mouse is within the host button click
		else if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 550 && Game::s_mousePosY <= 595 && Game::s_buttonState == HOSTDOWN)
		{
			Game::s_gameState = GAME;
		}

		// The mouse is within the quit button click
		else if (Game::s_mousePosX >= 635.0f && Game::s_mousePosX <= 780.0f && Game::s_mousePosY >= 650 && Game::s_mousePosY <= 695 && Game::s_buttonState == QUITDOWN)
		{
			Game::s_gameState = GAME;
		}
		else
		{
			Game::s_buttonState = NOBUTTONDOWN;
		}
	}
}

Game::Game(GLFWwindow* window)
	: m_mousePosLabel("Basic Game", "Assets/Fonts/NYCTALOPIATILT.TTF")
{
	m_clock.Process();
	m_window = window;

	glfwSetMouseButtonCallback(window, &glfwGetMouseButtonCallBack);

	m_mousePosLabel.setPosition(glm::vec2(10.0f, 10.0f));
	m_mousePosLabel.setColor(glm::vec3(0.8f, 0.8F, 0.8f));

	// Create the join text
	TextLabel join("Join", "Assets/Fonts/NYCTALOPIATILT.TTF");
	join.setPosition(glm::vec2(637.0f, 312.0f));
	join.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	Game::m_uiText.push_back(join);

	// Create the host text
	TextLabel host("Host", "Assets/Fonts/NYCTALOPIATILT.TTF");
	host.setPosition(glm::vec2(632.0f, 212.0f));
	host.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	Game::m_uiText.push_back(host);

	// Create the quit text
	TextLabel quit("Quit", "Assets/Fonts/NYCTALOPIATILT.TTF");
	quit.setPosition(glm::vec2(637.0f, 112.0f));
	quit.setColor(glm::vec3(0.8f, 0.8f, 0.8f));
	Game::m_uiText.push_back(quit);

}


Game::~Game()
{
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

	m_mousePosLabel.Render();

	// Render all the labels on screen;
	for (unsigned int i = 0; i < Game::m_uiText.size(); ++i)
	{
		Game::m_uiText.at(i).Render();
	}

	glfwSwapBuffers(m_window);
}

void Game::process(float deltaTick)
{
	// The Game is currently in the main menu.
	if (s_gameState == MAINMENU)
	{
		glfwGetCursorPos(m_window, &s_mousePosX, &s_mousePosY);

		// The mouse is within the join button click
		if (s_mousePosX >= 635.0f && s_mousePosX <= 780.0f && s_mousePosY >= 450 && s_mousePosY <= 495)
		{
			if(Game::s_buttonState == NOBUTTONDOWN)
				m_uiText.at(0).setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			else if (Game::s_buttonState == JOINDOWN)
				m_uiText.at(0).setColor(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
			m_uiText.at(0).setColor(glm::vec3(0.8f, 0.8f, 0.8f));

		// The mouse is within the host button click
		if (s_mousePosX >= 635.0f && s_mousePosX <= 780.0f && s_mousePosY >= 550 && s_mousePosY <= 595)
		{
			if(Game::s_buttonState == NOBUTTONDOWN)
				m_uiText.at(1).setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			else if (Game::s_buttonState == HOSTDOWN)
				m_uiText.at(1).setColor(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
			m_uiText.at(1).setColor(glm::vec3(0.8f, 0.8f, 0.8f));

		// The mouse is within the quit button click
		if (s_mousePosX >= 635.0f && s_mousePosX <= 780.0f && s_mousePosY >= 650 && s_mousePosY <= 695)
		{
			if(Game::s_buttonState == NOBUTTONDOWN)
				m_uiText.at(2).setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			else if (Game::s_buttonState == QUITDOWN)
				m_uiText.at(2).setColor(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
			m_uiText.at(2).setColor(glm::vec3(0.8f, 0.8f, 0.8f));

		render();
	}
	// The game is at the Lobby screen.
	else if (s_gameState == LOBBY)
	{

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
