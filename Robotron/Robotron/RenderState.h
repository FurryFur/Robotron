#pragma once

#include <GLFW\glfw3.h>
#include <glad\glad.h>

struct Entity;

struct RenderState {
	GLFWwindow* glContext;
	const Entity* cameraEntity;
	GLuint environmentMap;
	bool hasEnvironmentMap;
	GLuint uboUniforms;
	GLuint uniformBindingPoint;
};