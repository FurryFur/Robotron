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
#include "Audio.h"
#include "EntityUtils.h"

#include <GLFW\glfw3.h>
double oldTime = 0.0;
double newTime = 0.0;

int main()
{
	
	// Init combined Window and OpenGL context.
	GLFWwindow* window = GLUtils::initOpenGL();

	EntityUtils::preloadModelsAndTextures();

	Audio audio;
	audio.playBgMusic();

	bool resetGame = false;
	std::unique_ptr<Game> game;

	while (!glfwWindowShouldClose(window)) {		
		// Execute a from of the game.
		if (!resetGame)
		{
			if(game == nullptr)
				game = std::make_unique<Game>(window, audio);
			game->executeOneFrame();
			resetGame = game->checkReset();
		}
		else
		{
			game.reset(nullptr);
			resetGame = false;
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}