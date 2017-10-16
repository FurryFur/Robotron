//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : The main entry point for the program.
//                Handles executing the main loop.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#include "GLUtils.h"
#include "Level.h"

#include <GLFW\glfw3.h>

int main()
{
	//// Init combined Window and OpenGL context.
	GLFWwindow* window = GLUtils::initOpenGL();
	Scene scene;

	Level level(window, scene);


	while (!glfwWindowShouldClose(window)) {
		level.process();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}