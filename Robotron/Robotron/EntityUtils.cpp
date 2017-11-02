#define _USE_MATH_DEFINES

#include "EntityUtils.h"

#include "GLUtils.h"
#include "GLPrimitives.h"
#include "Scene.h"
#include "Entity.h"
#include "ModelUtils.h"

#include <glm\gtc\matrix_transform.hpp>
#include <iostream>

Entity& EntityUtils::createQuad(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM 
	                                  | COMPONENT_LOGIC);

	entity.transform = transform;


	entity.model = GLPrimitives::getQuadModel();

	setDefaultInputBindings(entity.inputMap);

	// Replace default texture
	entity.model.materials.at(0).colorMaps.at(0) = GLUtils::loadTexture("Assets/Textures/dessert-floor.png");

	entity.controlVars.maxMoveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	return entity;
}

Entity& EntityUtils::createSphere(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_INPUT_MAP | COMPONENT_MODEL 
	                                  | COMPONENT_TRANSFORM);

	entity.transform = transform;

	entity.model = GLPrimitives::getSphereModel();

	setDefaultInputBindings(entity.inputMap);

	entity.controlVars.maxMoveSpeed = 0.2f;
	entity.controlVars.maxAcceleration = 1.0f;
	entity.controlVars.worldSpaceMove = true;

	return entity;
}

Entity& EntityUtils::createEnemyZombie(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM
	                                  | COMPONENT_ZOMBIE
	                                  | COMPONENT_NETWORK | COMPONENT_PHYSICS);

	entity.aiVariables.score = 5;
	entity.aiVariables.lives = 1;

	float pi = static_cast<float>(M_PI_2);
	entity.transform = transform * glm::rotate(glm::mat4{}, pi, glm::vec3{ 1.0f, 0.0f, 0.0f });

	setModelEnemyZombie(entity);

	entity.controlVars.maxMoveSpeed = 10.0f;
	entity.controlVars.maxAcceleration = 20.0f;
	entity.controlVars.accelerationWeight = 1;
	entity.controlVars.worldSpaceMove = true;

	return entity;
}

Entity& EntityUtils::createEnemySnake(Scene& scene, const glm::mat4& transform, int positionInQueue)
{
	Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM 
	                                  | COMPONENT_SNAKE
	                                  | COMPONENT_NETWORK | COMPONENT_PHYSICS);
	
	entity.aiVariables.positionInQueue = positionInQueue;

	entity.aiVariables.score = 5;
	entity.aiVariables.lives = 1;

	float pi = static_cast<float>(M_PI_2);
	entity.transform = transform * glm::rotate(glm::mat4{}, pi, glm::vec3{ 1.0f, 0.0f, 0.0f });

	setModelEnemySnake(entity);

	entity.controlVars.maxMoveSpeed = 15;
	entity.controlVars.maxAcceleration = 100;
	entity.controlVars.accelerationWeight = 5;
	entity.controlVars.worldSpaceMove = true;

	return entity;
}

Entity& EntityUtils::createEnemyShooter(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM 
	                                  | COMPONENT_ENEMY_SHOOTER
	                                  | COMPONENT_NETWORK | COMPONENT_PHYSICS);
	float pi = static_cast<float>(M_PI_2);
	entity.transform = transform * glm::rotate(glm::mat4{}, pi, glm::vec3{ 1.0f, 0.0f, 0.0f });

	entity.aiVariables.score = 10;
	entity.aiVariables.lives = 3;

	setModelEnemyShooter(entity);

	entity.controlVars.maxMoveSpeed = 10.0f;
	entity.controlVars.maxAcceleration = 10.0f;
	entity.controlVars.accelerationWeight = 1;
	entity.controlVars.worldSpaceMove = true;
	entity.controlVars.firingSpeed = 1.0f;

	return entity;
}

Entity& EntityUtils::createScorePickUp01(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM
	                                  | COMPONENT_SCOREPICKUP
	                                  | COMPONENT_NETWORK | COMPONENT_PHYSICS);

	entity.transform = transform;

	entity.aiVariables.score = 10;

	setModelScorePickup01(entity);

	entity.controlVars.maxMoveSpeed = 20.0f;
	entity.controlVars.maxAcceleration = 100.0f;
	entity.controlVars.accelerationWeight = 2.0f;
	entity.controlVars.worldSpaceMove = true;

	return entity;
}

Entity& EntityUtils::createScorePickUp02(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = createScorePickUp01(scene, transform);

	entity.aiVariables.score = 20;

	setModelScorePickUp02(entity);

	return entity;
}

Entity& EntityUtils::createHealthPickUp(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = createScorePickUp01(scene, transform);

	entity.aiVariables.lifePickUp = true;

	setModelHealthPickUp(entity);

	return entity;
}


Entity& EntityUtils::createPlayer(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_INPUT | COMPONENT_PLAYER_CONTROL 
	                                  | COMPONENT_INPUT_MAP | COMPONENT_MODEL
	                                  | COMPONENT_TRANSFORM
	                                  | COMPONENT_NETWORK | COMPONENT_PHYSICS
									  | COMPONENT_PLAYER);

	entity.playerStats.playerInfo.lives = 100;
	entity.playerStats.playerInfo.score = 0;
	entity.playerStats.extraLifeThreshhold = 1000;
	entity.transform = transform * glm::scale(glm::mat4{}, { 5.0f, 5.0f, 5.0f }); // Scale the player model up by default

	setModelPlayer(entity);

	entity.inputMap = {};
	entity.inputMap.mouseInputEnabled = false;
	entity.inputMap.leftBtnMap = GLFW_KEY_A;
	entity.inputMap.rightBtnMap = GLFW_KEY_D;
	entity.inputMap.forwardBtnMap = GLFW_KEY_W;
	entity.inputMap.backwardBtnMap = GLFW_KEY_S;

	entity.controlVars.maxMoveSpeed = 8;
	entity.controlVars.maxAcceleration = 20;
	entity.controlVars.damping = 1;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;
	entity.controlVars.firingSpeed = 0.5f;

	return entity;
}

Entity& EntityUtils::createPlayerBullet(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM
	                                  | COMPONENT_PLAYERBULLET
	                                  | COMPONENT_NETWORK | COMPONENT_PHYSICS);

	entity.transform = transform;

	setModelPlayerBullet(entity);

	entity.controlVars.maxMoveSpeed = 10.0f;
	entity.controlVars.maxAcceleration = 10.0f;
	entity.controlVars.worldSpaceMove = true;

	return entity;
}

Entity& EntityUtils::createEnemyBullet(Scene& scene, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM
		| COMPONENT_ENEMYBULLET
		| COMPONENT_NETWORK | COMPONENT_PHYSICS);

	entity.transform = transform;

	setModelEnemyBullet(entity);

	entity.controlVars.maxMoveSpeed = 10.0f;
	entity.controlVars.maxAcceleration = 10.0f;
	entity.controlVars.accelerationWeight = 1;
	entity.controlVars.worldSpaceMove = true;

	return entity;
}

void EntityUtils::setModelEnemyZombie(Entity& entity)
{
	entity.model = GLPrimitives::getSphereModel();

	// Replace default texture
	entity.model.materials.at(0).colorMaps.at(0) = GLUtils::loadTexture("Assets/Textures/blackhole.jpg");
	entity.model.materials.at(0).shaderParams.glossiness = 0.3f;
	entity.model.materials.at(0).shaderParams.metallicness = 0.3f;
}

void EntityUtils::setModelEnemySnake(Entity& entity)
{
	entity.model = GLPrimitives::getSphereModel();

	// Replace default texture
	entity.model.materials.at(0).colorMaps.at(0) = GLUtils::loadTexture("Assets/Textures/doge.jpg");
	entity.model.materials.at(0).shaderParams.glossiness = 0.3f;
	entity.model.materials.at(0).shaderParams.metallicness = 0.3f;
}

void EntityUtils::setModelEnemyShooter(Entity& entity)
{
	entity.model = GLPrimitives::getSphereModel();

	// Replace default texture
	entity.model.materials.at(0).colorMaps.at(0) = GLUtils::loadTexture("Assets/Textures/coral.jpg");
	entity.model.materials.at(0).shaderParams.glossiness = 0.3f;
	entity.model.materials.at(0).shaderParams.metallicness = 0.3f;
}

void EntityUtils::setModelScorePickup01(Entity& entity)
{
	entity.model = GLPrimitives::getPyramidModel();

	// Replace default texture
	entity.model.materials.at(0).colorMaps.at(0) = GLUtils::loadTexture("Assets/Textures/green.jpg");
	entity.model.materials.at(0).shaderParams.glossiness = 1.0f;
	entity.model.materials.at(0).shaderParams.metallicness = 1.0f;
}

void EntityUtils::setModelScorePickUp02(Entity& entity)
{
	entity.model = GLPrimitives::getPyramidModel();

	// Replace default texture
	entity.model.materials.at(0).colorMaps.at(0) = GLUtils::loadTexture("Assets/Textures/green.jpg");
	entity.model.materials.at(0).shaderParams.glossiness = 1.0f;
	entity.model.materials.at(0).shaderParams.metallicness = 1.0f;
}

void EntityUtils::setModelHealthPickUp(Entity& entity)
{
	entity.model = GLPrimitives::getCubeModel();

	// Replace default texture
	entity.model.materials.at(0).colorMaps.at(0) = GLUtils::loadTexture("Assets/Textures/green.jpg");
	entity.model.materials.at(0).shaderParams.glossiness = 1.0f;
	entity.model.materials.at(0).shaderParams.metallicness = 1.0f;
}

void EntityUtils::setModelPlayer(Entity& entity)
{
	entity.model = ModelUtils::loadModel("Assets/Models/spaceship/space_ship.obj");

	for (size_t i = 0; i < entity.model.materials.size(); ++i)
	{
		entity.model.materials.at(i).shaderParams.glossiness = 1.0f;
	}
}

void EntityUtils::setModelPlayerBullet(Entity& entity)
{
	entity.model = GLPrimitives::getSphereModel();

	// Replace default texture
	entity.model.materials.at(0).colorMaps.at(0) = GLUtils::loadTexture("Assets/Textures/purple.png");
	entity.model.materials.at(0).shaderParams.glossiness = 1.0f;
	entity.model.materials.at(0).shaderParams.metallicness = 1.0f;
}

void EntityUtils::setModelEnemyBullet(Entity& entity)
{
	entity.model = GLPrimitives::getSphereModel();

	// Replace default texture
	entity.model.materials.at(0).colorMaps.at(0) = GLUtils::loadTexture("Assets/Textures/red.png");
	entity.model.materials.at(0).shaderParams.glossiness = 0.5f;
	entity.model.materials.at(0).shaderParams.metallicness = 1.0f;
}

Entity& EntityUtils::createGhost(Scene& scene, ModelID modelId, const glm::mat4& transform, std::int32_t entityNetId)
{
	Entity& entity = scene.createEntity(COMPONENT_MODEL, COMPONENT_TRANSFORM, 
	                                    COMPONENT_NETWORK, COMPONENT_PHYSICS);

	switch (modelId)
	{
		// TODO: Replace all these with calls to a create model functions.
		// That way we can easily change the models and textures used in a single location.
	case MODEL_PLAYER_BULLET:
		setModelPlayerBullet(entity);
		break;
	case MODEL_ENEMY_BULLET:
		setModelEnemyBullet(entity);
		break;
	case MODEL_ENEMY_ZOMBIE:
		setModelEnemyZombie(entity);
		break;
	case MODEL_ENEMY_SNAKE:
		setModelEnemySnake(entity);
		break;
	case MODEL_ENEMY_SHOOTER:
		setModelEnemyShooter(entity);
		break;
	case MODEL_SCORE_PICKUP_1:
		setModelScorePickup01(entity);
		break;
	case MODEL_SCORE_PICKUP_2:
		setModelScorePickUp02(entity);
		break;
	case MODEL_HEALTH_PICKUP:
		setModelHealthPickUp(entity);
		break;
	default:
		// TODO: Add logging here.
		std::cout << "Error: Invalid modelID when trying to create entity ghost" << std::endl;
		break;
	}

	entity.transform = transform;
	entity.network.id = entityNetId;

	return entity;
}

Entity & EntityUtils::createPlayerGhost(Scene& scene, const glm::mat4& transform, std::int32_t entityNetId)
{
	Entity& entity = createPlayer(scene, transform);
	entity.removeComponents(COMPONENT_PLAYER_CONTROL);
	entity.removeComponents(COMPONENT_INPUT_MAP);
	entity.network.id = entityNetId;
	return entity;
}

Entity& EntityUtils::createCylinder(Scene& scene, float radius, float height, const glm::mat4& transform)
{
	Entity& entity = scene.createEntity(COMPONENT_INPUT_MAP | COMPONENT_MODEL 
	                                  | COMPONENT_TRANSFORM | COMPONENT_LOGIC);

	entity.transform = transform * glm::scale(glm::mat4{ 1 }, glm::vec3{ radius, height, radius });

	entity.model = GLPrimitives::getCylinderModel();

	setDefaultInputBindings(entity.inputMap);

	entity.controlVars.maxMoveSpeed = 0.1f;
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

	entity.controlVars.maxMoveSpeed = 0.1f;
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

	entity.controlVars.maxMoveSpeed = 0.1f;
	entity.controlVars.orientationSensitivity = 0.05f;
	entity.controlVars.worldSpaceMove = true;

	entity.logicVars.rotationAxis = glm::vec3{ 0, 1, 0 };

	return entity;
}

Entity& EntityUtils::createCamera(Scene& scene, const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up)
{
	Entity& entity = scene.createEntity(COMPONENT_INPUT_MAP | COMPONENT_INPUT 
	                                  | COMPONENT_CAMERA | COMPONENT_TRANSFORM);

	entity.controlVars.maxMoveSpeed = 0.1f;
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
	entity.model.materials.at(0).colorMaps.push_back(GLUtils::loadCubeMapFaces(faceFilenames));
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


