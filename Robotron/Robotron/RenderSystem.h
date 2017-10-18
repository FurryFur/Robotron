//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles rendering each entity.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <vector>

struct Scene;
struct GLFWwindow;
struct Entity;

class RenderSystem {
public:
	RenderSystem(GLFWwindow* glContext, Scene&);
	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;

	// Draws a debugging arrow
	// This object will only be drawn once.
	// To keep or update the arrow, drawDebugArrow must be called every frame.
	static void drawDebugArrow(Scene&, const glm::vec3& base, const glm::vec3& tip);

	// Draws a debugging arrow
	// This object will only be drawn once.
	// To keep or update the arrow, drawDebugArrow must be called every frame.
	static void drawDebugArrow(Scene&, const glm::vec3& base, const glm::vec3& direction, float magnitude);

	// Starts rendering the frame.
	// Should be called before update.
	void beginRender();

	// Renders an entity.
	void update(Entity&);

	// Ends the frame.
	void endRender();

	// Sets the current camera.
	void setCamera(const Entity*);

	// Sets the environment map for reflections
	void setEnvironmentMap(const Entity&);

private:
	GLFWwindow* m_glContext;
	Scene& m_scene;
	const Entity* m_cameraEntity;
	GLuint m_uboUniforms;
	GLuint m_uboShaderParams;
	GLuint m_uniformBindingPoint;
	GLuint m_shaderParamsBindingPoint;

	// Handler to a cube map on the GPU, used for reflections and environmental lighting
	GLuint m_environmentMap;
	bool m_isEnvironmentMap;
};