#pragma once

#include <GLFW\glfw3.h>
#include <glad\glad.h>

class Entity;

struct RenderState {
	GLFWwindow* glContext;
	const Entity* cameraEntity;
	GLuint radianceMap;
	bool hasRadianceMap;
	GLuint irradianceMap;
	bool hasIrradianceMap;
	GLuint uboUniforms;
	GLuint uniformBindingPoint;
};