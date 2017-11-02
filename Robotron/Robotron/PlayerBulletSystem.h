//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles behaviour
//				  for player bullet entity types. They
//				  travel in a straight line until they
//				  hit an enemy. they kill it and themselves. 
//				  or they
//				  hit a wall and get destroyed
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#pragma once

#include "Audio.h"

class Scene;
class Entity;

class PlayerBulletSystem {
public:
	PlayerBulletSystem(Scene& scene, Audio audio);

	// Updates the entities position from input.
	void update(Entity&);

private:
	Scene& m_scene;
	Audio m_audio;
};