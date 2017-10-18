//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A collection of functions used to create new entities.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <vector>
#include <string>

struct Scene;
struct Entity;
struct VertexFormat;
struct Mesh;
struct InputComponent;
struct InputMapComponent;

namespace EntityUtils {
	// Creates a unit square facing down the positive z axis with the 
	// specified transform
	Entity& createQuad(Scene&, const glm::mat4& transform = glm::mat4{ 1 });

	// Creates a unit sphere, centered at the origin, with the specified 
	// transform;
	Entity& createSphere(Scene&, const glm::mat4& transform = glm::mat4{ 1 });

	Entity& createEnemy01(Scene &, const glm::mat4 & _transform = glm::mat4{ 1 });

	// Creates a  Player (currently a unit sphere), centered at the origin, with the specified 
	// transform;
	Entity& createPlayer(Scene&, const glm::mat4& transform = glm::mat4{ 1 });

	// Creates a cylinder with the specified radius and height.
	Entity& createCylinder(Scene&, float radius = 1, float height = 1, const glm::mat4& transform = glm::mat4{ 1 });

	// Creates a pyramid
	Entity& createPyramid(Scene&, const glm::mat4& transform = glm::mat4{ 1 });

	// Creates a pyramid
	Entity& createCube(Scene&, const glm::mat4& transform = glm::mat4{ 1 });

	// Creates a camera.
	// This camera needs to be set as active on the render in order to be rendered from.
	Entity& createCamera(Scene&, const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up = glm::vec3{ 0, 1, 0 });

	// Creates a skybox.
	// Can be used to set the environment map for the renderer.
	Entity& createSkybox(Scene&, const std::vector<std::string>& faceFilenames);

	// Creates an entity from a 3D model file.
	// The entity returned is a simple entity with only a model and a transform component.
	Entity& createModel(Scene&, const std::string& path, const glm::mat4& transform = glm::mat4{ 1 });
	
	// Handles boilerplate input binding
	void setDefaultInputBindings(InputMapComponent& input);
}