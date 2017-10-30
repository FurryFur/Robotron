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
#include "Game.h"

#include <GLFW\glfw3.h>

double oldTime = 0.0;
double newTime = 0.0;

int main()
{
	// Init combined Window and OpenGL context.
	GLFWwindow* window = GLUtils::initOpenGL();

	Game game(window);

	while (!glfwWindowShouldClose(window)) {		
		// Execute a from of the game.
		game.executeOneFrame();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}