//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles behaviour
//				  for enemy bullet entity types. They
//				  travel in a straight line until they
//				  hit a play. they kill it and themselves. 
//				  or they
//				  hit a wall and get destroyed
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#pragma once

class Scene;
class Entity;

class EnemyBulletSystem {
public:
	EnemyBulletSystem(Scene& scene);

	// Updates the entities position from input.
	void update(Entity&, float deltaTick);

private:
	Scene& m_scene;
};