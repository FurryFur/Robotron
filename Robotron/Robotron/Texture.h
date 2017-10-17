#pragma once

#include <glad\glad.h>

enum TextureType {
	TEXTURE_TYPE_DIFFUSE,
	TEXTURE_TYPE_SPECULAR,
	TEXTURE_TYPE_GLOSS
};

struct Texture {
	GLuint id;
	TextureType type;
	GLenum target;
};