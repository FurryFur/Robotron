//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A container for all the entities / components in 
//                scene.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include "Entity.h"

#include <vector>

struct Scene {
public:
	Entity& createEntity();
	void destroyEntity(Entity&);

	std::vector<Entity> entities;
};