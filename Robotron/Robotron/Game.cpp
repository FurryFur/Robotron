#include "Game.h"



Game::Game(GLFWwindow* window)
{
	m_clock.Process();
	m_window = window;
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

void Game::process(float deltaTick)
{
	if (m_level == NULL)
		m_level = new Level(m_window);

	m_level->process(deltaTick, m_clock);

	if (!m_level->checkEnemiesAlive())
		m_level->triggerNextLevel();
}
