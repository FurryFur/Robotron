//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles movement based on an
//                entities input state.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include "Clock.h"

class Scene;
class Entity;

class PlayerControlSystem {
public:
	PlayerControlSystem(Scene& scene);

	// Updates the entities position from input
	void update(Entity& entity, Clock& clock);

private:
	Scene& m_scene;
};