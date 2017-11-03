//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles input and propogates
//                input to entities.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//


#include "InputSystem.h"

#include "KeyObserver.h"
#include "Scene.h"
#include "Clock.h"
#include "Level.h"
#include "Utils.h"

#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>

InputSystem::InputSystem(GLFWwindow* window, Scene& scene, Clock& clock)
	: m_window{ window }
	, m_scene{ scene }
	, m_clock{ clock }
{
	// Register input system as a listener for keyboard events
	glfwSetWindowUserPointer(window, this);
	auto keyFunc = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		InputSystem* inputSystem = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
		inputSystem->keyCallback(key, scancode, action, mods);
	};
	glfwSetKeyCallback(window, keyFunc);
}

void InputSystem::registerKeyObserver(IKeyObserver* observer)
{
	m_keyObservers.push_back(observer);
}

// Record Action keys here
void InputSystem::keyCallback(int key, int scancode, int action, int mods)
{
	// Close window on exit
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(m_window, GLFW_TRUE);
		return;
	}

	if (key == 321) {
		if(action == GLFW_PRESS)
			shootLeftDown = true;
		if(action == GLFW_RELEASE)
			shootLeftDown = false;
		return;
	}

	if (key == 322) {
		if (action == GLFW_PRESS)
			shootDown = true;
		if (action == GLFW_RELEASE)
			shootDown = false;
		return;
	}

	if (key == 323) {
		if (action == GLFW_PRESS)
			shootRightDown = true;
		if (action == GLFW_RELEASE)
			shootRightDown = false;
		return;
	}

	if (key == 324) {
		if (action == GLFW_PRESS)
			shootLeft = true;
		if (action == GLFW_RELEASE)
			shootLeft = false;
		return;
	}

	if (key == 326) {
		if (action == GLFW_PRESS)
			shootRight = true;
		if (action == GLFW_RELEASE)
			shootRight = false;
		return;
	}

	if (key == 327) {
		if (action == GLFW_PRESS)
			shootLeftUp = true;
		if (action == GLFW_RELEASE)
			shootLeftUp = false;
		return;
	}

	if (key == 328) {
		if (action == GLFW_PRESS)
			shootUp = true;
		if (action == GLFW_RELEASE)
			shootUp = false;
		return;
	}

	if (key == 329) {
		if (action == GLFW_PRESS)
			shootRightUp = true;
		if (action == GLFW_RELEASE)
			shootRightUp = false;
		return;
	}

	for (auto& observer : m_keyObservers)
		observer->keyCallback(key, scancode, action, mods);
}

void InputSystem::beginFrame()
{
	static glm::dvec2 lastMousePos;

	// Set previous mouse pos to current mouse pos on first run
	static bool firstRun = true;
	if (firstRun) {
		glfwGetCursorPos(m_window, &lastMousePos.x, &lastMousePos.y);
		firstRun = false;
	}

	// Update input from mouse
	glm::dvec2 mousePos;
	glfwGetCursorPos(m_window, &mousePos.x, &mousePos.y);

	// Update mouse delta
	m_mouseDelta = mousePos - lastMousePos;

	lastMousePos = mousePos;
}

void InputSystem::update(Entity& entity)
{
	// DEBUG!!!
	if (entity.hasComponents(COMPONENT_MODEL)) {
		if (glfwGetKey(m_window, GLFW_KEY_KP_MULTIPLY) == GLFW_PRESS) {
			for (size_t i = 0; i < entity.model.materials.size(); ++i) {
				entity.model.materials.at(i).shaderParams.metallicness = clamp(entity.model.materials.at(i).shaderParams.metallicness + 0.01f, 0.001f, 1.0f);
			}
		}
		if (glfwGetKey(m_window, GLFW_KEY_KP_DIVIDE) == GLFW_PRESS) {
			for (size_t i = 0; i < entity.model.materials.size(); ++i) {
				entity.model.materials.at(i).shaderParams.metallicness = clamp(entity.model.materials.at(i).shaderParams.metallicness - 0.01f, 0.001f, 1.0f);
			}
		}
		if (glfwGetKey(m_window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
			for (size_t i = 0; i < entity.model.materials.size(); ++i) {
				entity.model.materials.at(i).shaderParams.glossiness = clamp(entity.model.materials.at(i).shaderParams.glossiness + 0.01f, 0.0001f, 1.0f);
			}
		}
		if (glfwGetKey(m_window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
			for (size_t i = 0; i < entity.model.materials.size(); ++i) {
				entity.model.materials.at(i).shaderParams.glossiness = clamp(entity.model.materials.at(i).shaderParams.glossiness - 0.01f, 0.0001f, 1.0f);
			}
		}
		if (glfwGetKey(m_window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
			for (size_t i = 0; i < entity.model.materials.size(); ++i) {
				entity.model.materials.at(i).shaderParams.specBias = clamp(entity.model.materials.at(i).shaderParams.specBias + 0.01f, 0.0f, 0.96f);
			}
		}
		if (glfwGetKey(m_window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
			for (size_t i = 0; i < entity.model.materials.size(); ++i) {
				entity.model.materials.at(i).shaderParams.specBias = clamp(entity.model.materials.at(i).shaderParams.specBias - 0.01f, 0.0f, 0.9599f);
			}
		}
	}

	if (entity.hasComponents(COMPONENT_PLAYER, COMPONENT_INPUT, COMPONENT_INPUT_MAP)) {
		// Triggers the player object to fire a bullet in the direction pressed by the player
		entity.input.shootLeftDown = shootLeftDown;
		entity.input.shootDown = shootDown;
		entity.input.shootRightDown = shootRightDown;
		entity.input.shootLeft = shootLeft;
		entity.input.shootRight = shootRight;
		entity.input.shootLeftUp = shootLeftUp;
		entity.input.shootUp = shootUp;
		entity.input.shootRightUp = shootRightUp;
	}

	// Filter input receivers
	const size_t kInputReceiverMask = COMPONENT_INPUT | COMPONENT_INPUT_MAP;
	if (!entity.hasComponents(kInputReceiverMask))
		return;

	InputComponent& input = entity.input;
	InputMapComponent& inputMap = entity.inputMap;

	// Update input from mouse
	input.orientationDelta = {};
	if (inputMap.mouseInputEnabled)
		input.orientationDelta = glm::vec3{ m_mouseDelta, 0 };

	// Update input from buttons
	input.axis = {};
	if (inputMap.leftBtnMap && glfwGetKey(m_window, inputMap.leftBtnMap) == GLFW_PRESS)
		input.axis.x -= 1;
	if (inputMap.rightBtnMap && glfwGetKey(m_window, inputMap.rightBtnMap) == GLFW_PRESS)
		input.axis.x += 1;
	if (inputMap.forwardBtnMap && glfwGetKey(m_window, inputMap.forwardBtnMap) == GLFW_PRESS)
		input.axis.z -= 1;
	if (inputMap.backwardBtnMap && glfwGetKey(m_window, inputMap.backwardBtnMap) == GLFW_PRESS)
		input.axis.z += 1;
	if (inputMap.downBtnMap && glfwGetKey(m_window, inputMap.downBtnMap) == GLFW_PRESS)
		input.axis.y -= 1;
	if (inputMap.upBtnMap && glfwGetKey(m_window, inputMap.upBtnMap) == GLFW_PRESS)
		input.axis.y += 1;
	if (inputMap.azimuthPosBtnMap && glfwGetKey(m_window, inputMap.azimuthPosBtnMap) == GLFW_PRESS)
		input.orientationDelta.x += 1;
	if (inputMap.azimuthNegBtnMap && glfwGetKey(m_window, inputMap.azimuthNegBtnMap) == GLFW_PRESS)
		input.orientationDelta.x -= 1;
	if (inputMap.elevationPosBtnMap && glfwGetKey(m_window, inputMap.elevationPosBtnMap) == GLFW_PRESS)
		input.orientationDelta.y += 1;
	if (inputMap.elevationNegBtnMap && glfwGetKey(m_window, inputMap.elevationNegBtnMap) == GLFW_PRESS)
		input.orientationDelta.y -= 1;
	if (inputMap.rollBtnMap && glfwGetKey(m_window, inputMap.rollBtnMap) == GLFW_PRESS)
		input.orientationDelta.z += 1;
	if (inputMap.btn1Map)
		input.btn1Down = glfwGetKey(m_window, inputMap.btn1Map) == GLFW_PRESS;
	if (inputMap.btn2Map)
		input.btn2Down = glfwGetKey(m_window, inputMap.btn2Map) == GLFW_PRESS;
	if (inputMap.btn3Map)
		input.btn3Down = glfwGetKey(m_window, inputMap.btn3Map) == GLFW_PRESS;
	if (inputMap.btn4Map)
		input.btn4Down = glfwGetKey(m_window, inputMap.btn4Map) == GLFW_PRESS;
}