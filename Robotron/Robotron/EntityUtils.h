//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A collection of functions used to create new entities.
// Author       : Lance Chaney, Jack Mair
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include "TransformComponent.h"

#include <glm\glm.hpp>
#include <glad\glad.h>

#include <vector>
#include <string>
#include <cstdint>

class Scene;
class Entity;
struct VertexFormat;
struct Mesh;
struct InputComponent;
struct InputMapComponent;
class PlayerInfo;

enum ModelID : std::uint8_t {
	MODEL_PLAYER_BULLET,
	MODEL_ENEMY_BULLET,
	MODEL_ENEMY_ZOMBIE,
	MODEL_ENEMY_SNAKE,
	MODEL_ENEMY_SHOOTER,
	MODEL_SCORE_PICKUP_1,
	MODEL_SCORE_PICKUP_2,
	MODEL_HEALTH_PICKUP
};

namespace EntityUtils {
	// Creates a unit square facing down the positive z axis with the 
	// specified lookAt
	Entity& createQuad(Scene&, const TransformComponent& transform = {});

	// Creates a unit sphere, centered at the origin, with the specified 
	// lookAt;
	Entity& createSphere(Scene&, const TransformComponent& transform = {});

	//Creates the enemy01 enemy type
	Entity& createEnemyZombie(Scene &, const TransformComponent& transform = {});

	//Creates the enemy02 enemy type
	Entity& createEnemySnake(Scene& scene, int positionInQueue, const TransformComponent& transform = {});

	//Creates the enemy03 enemy type
	Entity& createEnemyShooter(Scene& scene, const TransformComponent& transform = {});

	//Creates the score pickup01 entity type
	Entity& createScorePickUp01(Scene& scene, const TransformComponent& transform = {});

	//Creates the score pickup01 entity type
	Entity& createScorePickUp02(Scene& scene, const TransformComponent& transform = {});

	//Creates the score pickup01 entity type
	Entity& createHealthPickUp(Scene& scene, const TransformComponent& transform = {});

	// Creates a  Player (currently a unit sphere), centered at the origin, with the specified 
	// lookAt;
	Entity& createPlayer(Scene& scene, const TransformComponent& transform = {});

	// Creates a  Player Bullet (currently a unit sphere), centered at the origin, with the specified 
	// lookAt;
	Entity& createPlayerBullet(Scene& scene, const TransformComponent& transform = {});

	// Creates a  Player Bullet (currently a unit sphere), centered at the origin, with the specified 
	// lookAt;
	Entity& createEnemyBullet(Scene&, const TransformComponent& transform = {});

	void setModelEnemyZombie(Entity&);
	void setModelEnemySnake(Entity&);
	void setModelEnemyShooter(Entity&);
	void setModelScorePickup01(Entity&);
	void setModelScorePickUp02(Entity&);
	void setModelHealthPickUp(Entity&);
	void setModelPlayer(Entity&);
	void setModelPlayerBullet(Entity&);
	void setModelEnemyBullet(Entity&);

	// Create a simple dummy entity that will be controlled by a remote controller
	Entity& createGhost(Scene&, ModelID, const TransformComponent&, std::int32_t entityNetId);

	// Create local player object that will have it's inputs sent to a remote controller
	Entity& createPlayerGhost(Scene&, const PlayerInfo&, const TransformComponent&, std::int32_t entityNetId);

	// Creates a cylinder with the specified radius and height.
	Entity& createCylinder(Scene&, float radius = 1, float height = 1, const TransformComponent& transform = {});

	// Creates a pyramid
	Entity& createPyramid(Scene&, const TransformComponent& transform = {});

	// Creates a pyramid
	Entity& createCube(Scene&, const TransformComponent& transform = {});

	// Creates a camera.
	// This camera needs to be set as active on the render in order to be rendered from.
	Entity& createCamera(Scene&, const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up = glm::vec3{ 0, 1, 0 });

	// Creates a skybox.
	// Can be used to set the environment map for the renderer.
	Entity& createSkybox(Scene&, const std::vector<std::string>& faceFilenames);

	// Creates an entity from a 3D model file.
	// The entity returned is a simple entity with only a model and a lookAt component.
	Entity& createModel(Scene&, const std::string& path, const TransformComponent& transform = {});
	
	// Handles boilerplate input binding
	void setDefaultInputBindings(InputMapComponent& input);
}