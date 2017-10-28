//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles movement ai
//				  for Enemy01 enemy types. They
//				  around the arena and pursue with
//				  flocking the closest player in their
//				  aggro range.
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#pragma once

class Scene;
class Entity;

class Enemy01ControlSystem {
public:
	Enemy01ControlSystem(Scene& scene);

	// Updates the entities position from input
	void update(Entity&, float deltaTick);

private:
	Scene& m_scene;
};