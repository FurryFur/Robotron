#pragma once
#include "NetworkSystem.h"


class NetworkClientSystem : public NetworkSystem
{
public:
	NetworkClientSystem(Scene& scene);

	virtual void beginFrame() override;
	virtual void update(Entity&) override;

private:
	void destroyIfExists(size_t entityNetID);
};

