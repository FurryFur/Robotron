#pragma once
#include "NetworkSystem.h"


class NetworkClientSystem : public NetworkSystem
{
public:
	NetworkClientSystem(Scene& scene);

	virtual void beginFrame() override;
	virtual void update(size_t entityID) override;
};

