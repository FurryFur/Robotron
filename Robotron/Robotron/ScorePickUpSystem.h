//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles movement ai
//				  for score pickups entity types. They
//				  follow a player who gets close to them do
//			      only stop following if they die.
// Author       : Jack Mair
// Mail         : jack.mai7246@mediadesign.school.nz
//

#pragma once

struct Scene;
struct Entity;

class ScorePickUpSystem {
public:
	ScorePickUpSystem(Scene& scene);

	// Updates the entities position from input
	void update(Entity&, float deltaTick);

private:
	Scene& m_scene;
};