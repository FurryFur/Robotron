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

#include "Utils.h"
#include "GLUtils.h"
#include "SceneUtils.h"
#include "InputSystem.h"
#include "PlayerControlSystem.h"
#include "RenderSystem.h"
#include "Scene.h"
#include "NetworkSystem.h"
#include "NetworkServerSystem.h"
#include "GameplayLogicSystem.h"

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <cmath>
#include <memory>

#pragma once
class Level
{
public:
	Level(GLFWwindow* window, int levelNum);
	~Level();

	void process();

private:
	Scene m_scene;
	GLFWwindow* m_window;
	RenderSystem m_renderSystem;
	PlayerControlSystem m_movementSystem;
	InputSystem m_inputSystem;
	std::unique_ptr<NetworkSystem> m_networkSystem;

	//stores the level current on. effects enemy spawn rate and spawn positioning
	int m_levelNum;
};

