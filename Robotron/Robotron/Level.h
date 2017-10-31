//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Controls the gameplay scene and components
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
#include "NetworkSystem.h"
#include "NetworkServerSystem.h"
#include "GameplayLogicSystem.h"
#include "Enemy01ControlSystem.h"
#include "Enemy02ControlSystem.h"
#include "Enemy03ControlSystem.h"
#include "ScorePickUpSystem.h"
#include "PlayerBulletSystem.h"
#include "EnemyBulletSystem.h"
#include "PhysicsSystem.h"

#include "TextLabel.h"

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <cmath>
#include <memory>

#pragma once
class Level
{
public:
	Level(GLFWwindow* window, Clock& clock);
	~Level();

	// Increases the level count of the game and spawns the next enemies.
	void initalizeNextLevel();
	// Returns true if there are still alive enemies in the level.
	bool checkEnemiesAlive();
	void process(float _fDeltaTick, Clock& clock);
	void triggerNextLevel();

private:
	
	// Respawn players a moment after death time if they still have lives left. 
	void respawnDeadPlayers(Clock& clock);
	void spawnEnemies(int levelType);
	void processSetUpPhase();

	Scene m_scene;
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
	std::unique_ptr<NetworkSystem> m_networkSystem;

	TextLabel m_playerHealth;
	TextLabel m_playerScore;

	//stores the level current on. effects enemy spawn rate and spawn positioning
	int m_levelNum;
	bool m_inSetupPhase;
	bool m_descendingPlayers;
	int m_setUpTick = 0;
};

