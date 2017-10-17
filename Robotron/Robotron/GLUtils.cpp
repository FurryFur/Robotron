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

#include "GLUtils.h"

#include "InputSystem.h"
#include "Material.h"
#include "Mesh.h"
#include "PlayerControlComponent.h"
#include "Scene.h"
#include "ShaderHelper.h"
#include "VertexFormat.h"
#include "stb_image.h"

#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>

#include <iostream>

#define BUFFER_OFFSET(i) ((GLvoid *)(i*sizeof(float)))

// Callback for handling glfw errors
void errorCallback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

// Handles glContext resize events
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

GLFWwindow* GLUtils::initOpenGL()
{
	glfwSetErrorCallback(errorCallback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	// Create opengl glContext and glContext
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* glContext = glfwCreateWindow(800, 800, "Simple Renderer", nullptr, nullptr);
	if (!glContext)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(glContext);

	// Register callbacks
	//glfwSetKeyCallback(glContext, keyCallback);
	glfwSetFramebufferSizeCallback(glContext, framebufferSizeCallback);

	// Load opengl functinos
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Configure glContext
	glfwSwapInterval(1);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Setup opengl viewport
	int width, height;
	glfwGetFramebufferSize(glContext, &width, &height);
	glViewport(0, 0, width, height);

	return glContext;
}

GLuint GLUtils::getDefaultShader()
{
	static GLuint s_shader;
	static bool s_shaderBuilt = false;

	if (!s_shaderBuilt) {
		compileAndLinkShaders(
			"Assets/Shaders/default_vert.glsl",
			"Assets/Shaders/default_frag.glsl",
			s_shader);
		s_shaderBuilt = true;
	}

	return s_shader;
}

GLuint GLUtils::getThresholdShader()
{
	static GLuint s_shader;
	static bool s_shaderBuilt = false;

	if (!s_shaderBuilt) {
		compileAndLinkShaders(
			"Assets/Shaders/default_vert.glsl",
			"Assets/Shaders/threshold_frag.glsl",
			s_shader);
		s_shaderBuilt = true;
	}

	return s_shader;
}

GLuint GLUtils::getSkyboxShader()
{
	static GLuint s_shader;
	static bool s_shaderBuilt = false;

	if (!s_shaderBuilt) {
		compileAndLinkShaders(
			"Assets/Shaders/skybox_vert.glsl",
			"Assets/Shaders/skybox_frag.glsl",
			s_shader);
		s_shaderBuilt = true;
	}

	return s_shader;
}

GLuint GLUtils::bufferVertices(const std::vector<VertexFormat>& vertices, const std::vector<GLuint>& indices)
{
	GLuint VAO;
	GLuint buffers[2];
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &buffers[0]);
	glGenBuffers(1, &buffers[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
	
	GLuint positionLoc = 0;
	GLuint normalLoc = 1;
	GLuint texCoordLoc = 2;
	glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), BUFFER_OFFSET(0));
	glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), BUFFER_OFFSET(3));
	glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), BUFFER_OFFSET(6));

	glEnableVertexAttribArray(positionLoc);
	glEnableVertexAttribArray(normalLoc);
	glEnableVertexAttribArray(texCoordLoc);

	return VAO;
}


Texture GLUtils::loadTexture(const std::string& filename, TextureType textureType)
{
	// TODO: Make cached so duplicate textures aren't loaded to the GPU

	Texture texture;
	texture.target = GL_TEXTURE_2D;
	texture.type = textureType;

	int width, height, nrChannels;
	unsigned char* textureData = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

	glGenTextures(1, &texture.id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(texture.target, texture.id);
	
	glTexParameteri(texture.target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texture.target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(texture.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(texture.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(texture.target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
	glGenerateMipmap(texture.target);

	stbi_image_free(textureData);
	glBindTexture(texture.target, 0);

	return texture;
}

Texture GLUtils::loadCubeMap(const std::vector<std::string>& faceFilenames)
{
	Texture texture;
	texture.target = GL_TEXTURE_CUBE_MAP;
	texture.type = TEXTURE_TYPE_DIFFUSE;

	glGenTextures(1, &texture.id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(texture.target, texture.id);

	for (GLenum i = 0; i < faceFilenames.size(); ++i) {
		int width, height, nrChannels;
		unsigned char* faceData = stbi_load(faceFilenames.at(i).c_str(), &width, &height, 
		                                    &nrChannels, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
		             0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, faceData);

		stbi_image_free(faceData);
	}

	glTexParameteri(texture.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texture.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(texture.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texture.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(texture.target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(texture.target, 0);

	return texture;
}
