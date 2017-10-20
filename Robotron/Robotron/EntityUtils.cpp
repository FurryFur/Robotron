#define _USE_MATH_DEFINES

#include "EntityUtils.h"

#include "GLUtils.h"
#include "GLPrimitives.h"
#include "Scene.h"
#include "Entity.h"
#include "ModelUtils.h"

#include <glm\gtc\matrix_transform.hpp>

Entity& EntityUtils::createQuad(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM 
	                                  | COMPONENT_LOGIC);

	entity.transform = transform;

	entity.model = GLPrimitives::getQuadModel();

	setDefaultInputBindings(entity.inputMap);

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	entity.logicVars.rotationAxis = glm::vec3{ 0, 0, 1 };

	return entity;
}

Entity& EntityUtils::createSphere(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_INPUT_MAP | COMPONENT_MODEL 
	                                  | COMPONENT_TRANSFORM | COMPONENT_LOGIC);

	entity.transform = transform;

	entity.model = GLPrimitives::getSphereModel();

	setDefaultInputBindings(entity.inputMap);

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	entity.logicVars.rotationAxis = glm::vec3{ 0, 1, 0 };

	return entity;
}

Entity& EntityUtils::createEnemy01(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM
	                                  | COMPONENT_LOGIC | COMPONENT_ENEMY01);

	entity.transform = transform;

	entity.model = GLPrimitives::getSphereModel();

	// Replace default texture
	entity.model.materials.at(0).colorMaps.at(0) = GLUtils::loadTexture("Assets/Textures/random-texture4.jpg");

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	entity.logicVars.rotationAxis = glm::vec3{ 0, 1, 0 };

	return entity;
}

Entity& EntityUtils::createPlayer(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_INPUT | COMPONENT_PLAYER_CONTROL 
	                                  | COMPONENT_INPUT_MAP | COMPONENT_MODEL
	                                  | COMPONENT_TRANSFORM | COMPONENT_LOGIC);

	entity.transform = transform;

	entity.model = GLPrimitives::getSphereModel();

	// Replace default texture
	entity.model.materials.at(0).colorMaps.at(0) = GLUtils::loadTexture("Assets/Textures/random-texture2.jpg");

	entity.inputMap = {};
	entity.inputMap.mouseInputEnabled = false;
	entity.inputMap.leftBtnMap = GLFW_KEY_A;
	entity.inputMap.rightBtnMap = GLFW_KEY_D;
	entity.inputMap.forwardBtnMap = GLFW_KEY_W;
	entity.inputMap.backwardBtnMap = GLFW_KEY_S;

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	entity.logicVars.rotationAxis = glm::vec3{ 0, 1, 0 };

	return entity;
}

Entity& EntityUtils::createCylinder(Scene& scene, float radius, float height, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_INPUT_MAP | COMPONENT_MODEL 
	                                  | COMPONENT_TRANSFORM | COMPONENT_LOGIC);

	entity.transform = transform * glm::scale(glm::mat4{ 1 }, glm::vec3{ radius, height, radius });

	entity.model = GLPrimitives::getCylinderModel();

	setDefaultInputBindings(entity.inputMap);

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	entity.logicVars.rotationAxis = glm::vec3{ 0, 1, 0 };

	return entity;
}

Entity& EntityUtils::createPyramid(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_INPUT_MAP | COMPONENT_MODEL 
	                                  | COMPONENT_TRANSFORM | COMPONENT_LOGIC);

	entity.transform = transform;

	entity.model = GLPrimitives::getPyramidModel();

	setDefaultInputBindings(entity.inputMap);

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	entity.logicVars.rotationAxis = glm::vec3{ 0, 1, 0 };

	return entity;
}

Entity& EntityUtils::createCube(Scene& scene, const glm::mat4 & transform)
{
	Entity& entity = scene.createEntity(COMPONENT_INPUT_MAP | COMPONENT_MODEL 
	                                  | COMPONENT_TRANSFORM | COMPONENT_LOGIC);

	entity.transform = transform;

	entity.model = GLPrimitives::getCubeModel();

	setDefaultInputBindings(entity.inputMap);

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	entity.logicVars.rotationAxis = glm::vec3{ 0, 1, 0 };

	return entity;
}

Entity& EntityUtils::createCamera(Scene& scene, const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up)
{
	Entity& entity = scene.createEntity(COMPONENT_INPUT_MAP | COMPONENT_INPUT 
	                                  | COMPONENT_CAMERA | COMPONENT_TRANSFORM);

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.005f;
	entity.controlVars.worldSpaceMove = false;

	entity.transform = glm::inverse(glm::lookAt(pos, center, up));

	entity.inputMap.mouseInputEnabled = true;
	entity.inputMap.forwardBtnMap = GLFW_KEY_W;
	entity.inputMap.leftBtnMap = GLFW_KEY_A;
	entity.inputMap.backwardBtnMap = GLFW_KEY_S;
	entity.inputMap.rightBtnMap = GLFW_KEY_D;
	entity.inputMap.downBtnMap = GLFW_KEY_Q;
	entity.inputMap.upBtnMap = GLFW_KEY_E;

	return entity;
}

Entity& EntityUtils::createSkybox(Scene& scene, const std::vector<std::string>& faceFilenames)
{
	Entity& entity = scene.createEntity(COMPONENT_MODEL);

	entity.model = GLPrimitives::getCubeModel();

	// Replace default material
	entity.model.materials.at(0) = {};
	entity.model.materials.at(0).shader = GLUtils::getSkyboxShader();
	entity.model.materials.at(0).colorMaps.push_back(GLUtils::loadCubeMap(faceFilenames));
	entity.model.materials.at(0).willDrawDepth = false;

	return entity;
}

Entity & EntityUtils::createModel(Scene& scene, const std::string& path, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);

	entity.model = ModelUtils::loadModel(path);

	entity.transform = transform;

	return entity;
}

void EntityUtils::setDefaultInputBindings(InputMapComponent& inputMap)
{
	inputMap = {};
	inputMap.leftBtnMap = GLFW_KEY_KP_4;
	inputMap.rightBtnMap = GLFW_KEY_KP_6;
	inputMap.forwardBtnMap = GLFW_KEY_KP_8;
	inputMap.backwardBtnMap = GLFW_KEY_KP_5;
	inputMap.downBtnMap = GLFW_KEY_KP_7;
	inputMap.upBtnMap = GLFW_KEY_KP_9;
	inputMap.azimuthPosBtnMap = GLFW_KEY_KP_1;
	inputMap.azimuthNegBtnMap = GLFW_KEY_KP_2;
	inputMap.elevationPosBtnMap = GLFW_KEY_KP_3;
	inputMap.elevationNegBtnMap = GLFW_KEY_KP_DECIMAL;
	inputMap.rollBtnMap = GLFW_KEY_KP_0;
	inputMap.btn1Map = GLFW_KEY_KP_ADD;
	inputMap.btn2Map = GLFW_KEY_KP_SUBTRACT;
	inputMap.btn3Map = GLFW_KEY_KP_MULTIPLY;
	inputMap.btn4Map = GLFW_KEY_KP_DIVIDE;
}


