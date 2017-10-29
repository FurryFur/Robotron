#pragma once

#include "NetworkSystem.h"
#include "GhostSnapshot.h"

#include <vector>
#include <memory>

class Scene;
class Entity;

class NetworkClientSystem : public NetworkSystem
{
public:
	NetworkClientSystem(Scene& scene);

	virtual void beginFrame() override;
	virtual void update(Entity&, float deltaTick) override;
	virtual void endFrame() override;

private:
	void destroyIfExists(size_t entityNetID);

	std::uint32_t m_lastSeqNumSeen;
	std::vector<std::unique_ptr<GhostSnapshot>> m_ghostSnapshots;
	sockaddr_in m_serverAddress;
};

