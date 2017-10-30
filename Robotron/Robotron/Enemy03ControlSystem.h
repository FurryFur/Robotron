//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles movement ai
//				  for Enemy03 enemy types. 
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

class Scene;
class Clock;
class Entity;

class Enemy03ControlSystem {
public:
	Enemy03ControlSystem(Scene& scene);

	// Updates the entities position from input
	void update(Entity&, float deltaTick, Clock& clock);

private:
	Scene& m_scene;
};