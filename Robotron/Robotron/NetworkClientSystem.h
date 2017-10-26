#pragma once
#include "NetworkSystem.h"

struct Scene;
struct Entity;

class NetworkClientSystem : public NetworkSystem
{
public:
	NetworkClientSystem(Scene& scene);

	virtual void beginFrame() override;
	virtual void update(Entity&) override;
	virtual void endFrame() override;

private:
	void destroyIfExists(size_t entityNetID);

	std::uint32_t m_lastSeqNumSeen;
};

