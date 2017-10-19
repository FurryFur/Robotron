//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A collection of OpenGL utilty functions.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include "Texture.h"

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <vector>
#include <string>

struct VertexFormat;
struct GLFWwindow;
struct Scene;
class InputSystem;

namespace GLUtils {
	// Initializes the window, opengl context and opengl function pointers
	GLFWwindow* initOpenGL();

	// Returns a handler to the default shader.
	// This function will build the shader if it is not already built.
	GLuint getDefaultShader();

	// Returns a handler to the debug shader.
	// This shader can draw meshes with a solid color.
	// Change the materials debug color property to set which color to use.
	// This function will build the shader if it is not already built.
	GLuint getDebugShader();

	// Returns a handler to the threshold shader.
	// This function will build the shader if it is not already built.
	GLuint getThresholdShader();

	// Returns a hander to the skybox shader.
	// This function will build the sahder if it is not already built.
	GLuint getSkyboxShader();

	// Buffers vertex and index data to the GPU.
	// Returns a handler the the VAO associated with the vertices / indices.
	GLuint bufferMeshData(const std::vector<VertexFormat>& vertices, const std::vector<GLuint>& indices);

	// Loads a texture to GPU memory.
	// Returns a handler to the GPU texture.
	Texture loadTexture(const std::string& paths);

	// Loads a cube map to GPU memory.
	// Returns a handler to the GPU cube map.
	Texture loadCubeMap(const std::vector<std::string>& facePaths);
}