#define _USE_MATH_DEFINES

#include "EntityUtils.h"

#include "GLUtils.h"
#include "GLPrimitives.h"
#include "Scene.h"
#include "Entity.h"

#include <glm\gtc\matrix_transform.hpp>

Entity& EntityUtils::createQuad(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity();
	entity.componentMask |= COMPONENT_MESH | COMPONENT_MATERIAL | COMPONENT_TRANSFORM | COMPONENT_LOGIC;

	entity.material.shader = GLUtils::getDefaultShader();
	entity.material.texture = GLUtils::loadTexture("Assets/Textures/random-texture3.png");
	entity.material.textureType = GL_TEXTURE_2D;
	entity.material.enableDepth = true;
	entity.material.shaderParams.metallicness = 1.0f;
	entity.material.shaderParams.glossiness = 75.0f; // TODO: Fix values getting messed up on the gpu when this is 0 for some reason

	setDefaultInputBindings(entity.inputMap);

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	entity.mesh = GLPrimitives::getQuadMesh();

	entity.logicVars.rotationAxis = glm::vec3{ 0, 0, 1 };

	return entity;
}

Entity& EntityUtils::createSphere(Scene& scene, const glm::mat4& _transform)
{
	Entity& entity = scene.createEntity();
	entity.componentMask |= COMPONENT_INPUT_MAP | COMPONENT_MESH | COMPONENT_MATERIAL | COMPONENT_TRANSFORM | COMPONENT_LOGIC;

	entity.transform = _transform;

	entity.material.shader = GLUtils::getDefaultShader();
	entity.material.texture = GLUtils::loadTexture("Assets/Textures/random-texture2.jpg");
	entity.material.textureType = GL_TEXTURE_2D;
	entity.material.enableDepth = true;
	entity.material.shaderParams.metallicness = 0.3f;
	entity.material.shaderParams.glossiness = 2.0f; // TODO: Fix values getting messed up on the gpu when this is 0 for some reason

	entity.mesh = GLPrimitives::getSphereMesh();

	setDefaultInputBindings(entity.inputMap);

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	entity.logicVars.rotationAxis = glm::vec3{ 0, 1, 0 };

	return entity;
}

Entity& EntityUtils::createEnemy01(Scene& scene, const glm::mat4& _transform)
{
	Entity& entity = scene.createEntity();
	entity.componentMask |= COMPONENT_MESH | COMPONENT_MATERIAL | COMPONENT_TRANSFORM | COMPONENT_LOGIC;

	entity.transform = _transform;

	entity.material.shader = GLUtils::getDefaultShader();
	entity.material.texture = GLUtils::loadTexture("Assets/Textures/random-texture4.jpg");
	entity.material.textureType = GL_TEXTURE_2D;
	entity.material.enableDepth = true;
	entity.material.shaderParams.metallicness = 0.3f;
	entity.material.shaderParams.glossiness = 2.0f; // TODO: Fix values getting messed up on the gpu when this is 0 for some reason

	entity.mesh = GLPrimitives::getSphereMesh();

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	entity.logicVars.rotationAxis = glm::vec3{ 0, 1, 0 };

	return entity;
}

Entity& EntityUtils::createPlayer(Scene& scene, const glm::mat4& _transform)
{
	Entity& entity = scene.createEntity();
	entity.componentMask |= COMPONENT_INPUT | COMPONENT_PLAYER_CONTROL | COMPONENT_INPUT_MAP | COMPONENT_MESH | COMPONENT_MATERIAL | COMPONENT_TRANSFORM | COMPONENT_LOGIC;

	entity.transform = _transform;

	entity.material.shader = GLUtils::getDefaultShader();
	entity.material.texture = GLUtils::loadTexture("Assets/Textures/random-texture2.jpg");
	entity.material.textureType = GL_TEXTURE_2D;
	entity.material.enableDepth = true;
	entity.material.shaderParams.metallicness = 0.3f;
	entity.material.shaderParams.glossiness = 2.0f; // TODO: Fix values getting messed up on the gpu when this is 0 for some reason

	entity.mesh = GLPrimitives::getSphereMesh();

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

Entity& EntityUtils::createCylinder(Scene& scene, float radius, float height, const glm::mat4& _transform)
{
	Entity& entity = scene.createEntity();
	entity.componentMask |= COMPONENT_INPUT_MAP | COMPONENT_MESH | COMPONENT_MATERIAL | COMPONENT_TRANSFORM | COMPONENT_LOGIC;

	entity.transform = _transform * glm::scale(glm::mat4{ 1 }, glm::vec3{ radius, height, radius });

	entity.material.shader = GLUtils::getThresholdShader();
	entity.material.texture = GLUtils::loadTexture("Assets/Textures/random-texture4.jpg");
	entity.material.textureType = GL_TEXTURE_2D;
	entity.material.enableDepth = true;
	entity.material.shaderParams.metallicness = 0.75f;
	entity.material.shaderParams.glossiness = 40.0f; // TODO: Fix values getting messed up on the gpu when this is 0 for some reason

	entity.mesh = GLPrimitives::getCylinderMesh();

	setDefaultInputBindings(entity.inputMap);

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	entity.logicVars.rotationAxis = glm::vec3{ 0, 1, 0 };

	return entity;
}

Entity& EntityUtils::createPyramid(Scene& scene, const glm::mat4& _transform)
{
	Entity& entity = scene.createEntity();
	entity.componentMask |= COMPONENT_INPUT_MAP | COMPONENT_MESH | COMPONENT_MATERIAL | COMPONENT_TRANSFORM | COMPONENT_LOGIC;

	entity.transform = _transform;

	entity.material.shader = GLUtils::getDefaultShader();
	entity.material.texture = GLUtils::loadTexture("Assets/Textures/random-texture.jpg");
	entity.material.textureType = GL_TEXTURE_2D;
	entity.material.enableDepth = true;
	entity.material.shaderParams.metallicness = 0.95f;
	entity.material.shaderParams.glossiness = 10.0f; // TODO: Fix values getting messed up on the gpu when this is 0 for some reason

	entity.mesh = GLPrimitives::getPyramidMesh();

	setDefaultInputBindings(entity.inputMap);

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	entity.logicVars.rotationAxis = glm::vec3{ 0, 1, 0 };

	return entity;
}

Entity& EntityUtils::createCube(Scene& scene, const glm::mat4 & _transform)
{
	Entity& entity = scene.createEntity();
	entity.componentMask |= COMPONENT_INPUT_MAP | COMPONENT_MESH | COMPONENT_MATERIAL | COMPONENT_TRANSFORM | COMPONENT_LOGIC;

	entity.transform = _transform;

	entity.material.shader = GLUtils::getDefaultShader();
	entity.material.texture = GLUtils::loadTexture("Assets/Textures/random-texture3.png");
	entity.material.textureType = GL_TEXTURE_2D;
	entity.material.enableDepth = true;
	entity.material.shaderParams.metallicness = 0.95f;
	entity.material.shaderParams.glossiness = 10.0f; // TODO: Fix values getting messed up on the gpu when this is 0 for some reason

	entity.mesh = GLPrimitives::getCubeMesh();

	setDefaultInputBindings(entity.inputMap);

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	entity.logicVars.rotationAxis = glm::vec3{ 0, 1, 0 };

	return entity;
}

Entity& EntityUtils::createCamera(Scene& scene, const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up)
{
	Entity& entity = scene.createEntity();

	entity.componentMask = COMPONENT_CAMERA | COMPONENT_TRANSFORM;

	entity.controlVars.moveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.005f;
	entity.controlVars.worldSpaceMove = false;

	entity.transform = glm::inverse(glm::lookAt(pos, center, up));

	return entity;
}

Entity& EntityUtils::createSkybox(Scene& scene, const std::vector<std::string>& faceFilenames)
{
	Entity& entity = scene.createEntity();

	entity.componentMask = COMPONENT_MATERIAL | COMPONENT_MESH;

	entity.material = {};
	entity.material.shader = GLUtils::getSkyboxShader();
	entity.material.texture = GLUtils::loadCubeMap(faceFilenames);
	entity.material.textureType = GL_TEXTURE_CUBE_MAP;
	entity.material.enableDepth = false;

	entity.mesh = GLPrimitives::getCubeMesh();

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


