#pragma once

#include <GLFW\glfw3.h>
#include <glad\glad.h>

struct Entity;

struct RenderState {
	GLFWwindow* glContext;
	const Entity* cameraEntity;
	GLuint reflectionMap;
	bool hasEnvironmentMap;
	GLuint irradianceMap;
	bool hasIrradianceMap;
	GLuint uboUniforms;
	GLuint uniformBindingPoint;
};