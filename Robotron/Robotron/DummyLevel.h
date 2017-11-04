//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : The client runs a dummy level that contorls their input and rendering the scene
//				  
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#pragma once

#include "Scene.h"
#include "clock.h"
#include "RenderSystem.h"
#include "PlayerStatsMenu.h"
#include "TextLabel.h"
#include "KeyObserver.h"
#include "InputSystem.h"
#include "NetworkSystem.h"
#include "PhysicsSystem.h"

class DummyLevel : public IKeyObserver
{
public:
	//DummyLevel(GLFWwindow* window, Clock& clock, Scene& scene, std::string username, std::uint8_t playerIDNum);
	DummyLevel(GLFWwindow* window, Clock& clock, Scene& scene, std::string username);
	~DummyLevel();

	void process(float deltaTick, Clock& clock, NetworkSystem& networkSystem);

	// Returns true if there are still alive players in the level.
	bool checkPlayersAlive();
	// Get the player's score
	int getPlayerScore();
private:
	Scene& m_scene;
	GLFWwindow* m_window;
	RenderSystem m_renderSystem;
	InputSystem m_inputSystem;
	PhysicsSystem m_physicsSystem;

	std::uint8_t m_playerIDNum;
	PlayerStatsMenu m_playerStatsMenu;

	TextLabel m_playerHealth;
	TextLabel m_playerScore;

	//stores the level current on. effects enemy spawn rate and spawn positioning
	bool m_drawConnectPlayerStats;

	// Inherited via IKeyObserver
	virtual void keyCallback(int key, int scancode, int action, int mods) override;
};

