#pragma once

#include "NetworkSystem.h"
#include "socket.h"

struct Scene;

class NetworkServerSystem : public NetworkSystem {
public:
	NetworkServerSystem(Scene&);

	virtual void beginFrame() override;
	virtual void update(Entity&) override;
};

