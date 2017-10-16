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

#include "GLUtils.h"
#include "SceneUtils.h"
#include "InputSystem.h"
#include "PlayerControlSystem.h"
#include "RenderSystem.h"
#include "Scene.h"
#include "NetworkSystem.h"
#include "GameplayLogicSystem.h"

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <cmath>

#pragma once
class Level
{
public:
	Level(GLFWwindow* window, Scene& scene);
	~Level();

	void process();

private:
	GLFWwindow* m_window;
	Scene& m_scene;
	RenderSystem m_renderSystem;
	PlayerControlSystem m_movementSystem;
	InputSystem m_inputSystem;
	GameplayLogicSystem m_gameplayLogicSystem;
	NetworkSystem m_networkSystem;
};

