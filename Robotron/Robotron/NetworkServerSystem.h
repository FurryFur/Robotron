#pragma once

#include "NetworkSystem.h"
#include "socket.h"

#include <vector>
#include <unordered_set>

struct Scene;
struct Entity;

class NetworkServerSystem : public NetworkSystem {
public:
	NetworkServerSystem(Scene&);

	virtual void beginFrame() override;
	virtual void update(Entity&) override;

private:
	void broadcastToClients(Packet packet);

	std::unordered_set<sockaddr_in> m_clients;
};

