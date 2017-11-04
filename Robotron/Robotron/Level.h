//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : The host runs the main level. The handles all the game logic and renders it for the host
//				  
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#define _USE_MATH_DEFINES

// Local Includes
#include "clock.h"
#include "Utils.h"
#include "GLUtils.h"
#include "EntityUtils.h"
#include "InputSystem.h"
#include "PlayerControlSystem.h"
#include "RenderSystem.h"
#include "Scene.h"
#include "GameplayLogicSystem.h"
#include "Enemy01ControlSystem.h"
#include "Enemy02ControlSystem.h"
#include "Enemy03ControlSystem.h"
#include "ScorePickUpSystem.h"
#include "PlayerBulletSystem.h"
#include "EnemyBulletSystem.h"
#include "PhysicsSystem.h"
#include "KeyObserver.h"
#include "TextLabel.h"
#include "Audio.h"
#include "PlayerStatsMenu.h"
#include "NetworkSystem.h"

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <cmath>
#include <memory>
#include <vector>

#pragma once
class Level : public IKeyObserver
{
public:
	Level(GLFWwindow* window, Clock& clock, Audio audio, Scene& scene, std::string username, NetworkSystem& networkSystem);
	~Level();

	// Increases the level count of the game and spawns the next enemies.
	void initalizeNextLevel();
	// Returns true if there are still alive enemies in the level.
	bool checkEnemiesAlive();
	// Returns true if there are still alive players in the level.
	bool checkPlayersAlive();
	void process(float deltaTick, Clock& clock);
	void triggerNextLevel();
	// Get the player's score
	int getPlayerScore();
private:
	
	// Respawn players a moment after death time if they still have lives left. 
	void respawnDeadPlayers(Clock& clock);
	void spawnEnemies(int levelType);
	void processSetUpPhase();

	Scene& m_scene;
	NetworkSystem& m_networkSystem;
	Audio m_audio;
	GLFWwindow* m_window;
	RenderSystem m_renderSystem;
	PlayerControlSystem m_playerControlSystem;
	InputSystem m_inputSystem;
	Enemy01ControlSystem m_enemy01ControlSystem;
	Enemy02ControlSystem m_enemy02ControlSystem;
	Enemy03ControlSystem m_enemy03ControlSystem;
	ScorePickUpSystem m_scorePickUpSystem;
	PlayerBulletSystem m_playerbulletsystem;
	EnemyBulletSystem m_enemybulletsystem;
	PhysicsSystem m_physicsSystem;

	PlayerStatsMenu m_playerStatsMenu;

	TextLabel m_playerHealth;
	TextLabel m_playerScore;

	//stores the level current on. effects enemy spawn rate and spawn positioning
	int m_levelNum;
	bool m_inSetupPhase;
	bool m_descendingPlayers;
	int m_setUpTick = 0;
	bool m_drawConnectPlayerStats;

	// Inherited via IKeyObserver
	virtual void keyCallback(int key, int scancode, int action, int mods) override;
};

