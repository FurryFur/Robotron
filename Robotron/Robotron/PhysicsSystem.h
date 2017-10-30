#pragma once

#include "Entity.h"

class Scene;

class PhysicsSystem {
public:
	PhysicsSystem(Scene&);

	void update(Entity&, float deltaTick);

private:
	Scene& m_scene;
};