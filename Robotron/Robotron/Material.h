//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A component for storing per entity
//                material information used by the GPU.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include "ShaderParams.h"
#include "Texture.h"

#include <glad\glad.h>

#include <vector>

struct Material {
	GLuint shader;
	ShaderParams shaderParams;
	std::vector<Texture> textures;
	bool willDrawDepth;
};