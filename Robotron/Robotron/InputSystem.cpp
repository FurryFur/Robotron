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
#include "Utils.h"

#include <GLFW\glfw3.h>

InputSystem::InputSystem(GLFWwindow* window, Scene& scene)
	: m_window{ window }
	, m_scene{ scene }
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
	if (entity.hasAllComponents(COMPONENT_MODEL)) {
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
				entity.model.materials.at(i).shaderParams.glossiness = clamp(entity.model.materials.at(i).shaderParams.glossiness + 0.1f, 0.001f, 1000.0f);
			}
		}
		if (glfwGetKey(m_window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
			for (size_t i = 0; i < entity.model.materials.size(); ++i) {
				entity.model.materials.at(i).shaderParams.glossiness = clamp(entity.model.materials.at(i).shaderParams.glossiness - 0.1f, 0.001f, 1000.0f);
			}
		}
	}

	// Filter input receivers
	const size_t kInputReceiverMask = COMPONENT_INPUT | COMPONENT_INPUT_MAP;
	if ((entity.componentMask & kInputReceiverMask) != kInputReceiverMask)
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