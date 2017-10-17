#pragma once

#include <glad\glad.h>
#include <assimp\material.h>

struct Texture {
	GLuint id;
	aiTextureType type;
	GLenum target;
};