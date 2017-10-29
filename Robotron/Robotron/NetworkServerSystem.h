#pragma once

#include "NetworkSystem.h"
#include "socket.h"
#include "RPC.h"
#include "ClientInfo.h"

#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <queue>

class Scene;
class Entity;
struct Packet;

class EntityPriorityComparitor {
public:
	bool operator()(const Entity* lhs, const Entity* rhs)
	{
		return lhs->network.priority < rhs->network.priority;
	}
};

class NetworkServerSystem : public NetworkSystem {
public:
	NetworkServerSystem(Scene&);

	virtual void beginFrame() override;
	virtual void update(Entity&, float deltaTick) override;
	virtual void endFrame() override;

private:
	// The time at which the last packet was sent
	std::chrono::time_point<std::chrono::high_resolution_clock> m_tLastPacketSent;

	// The interval between sending packets to clients
	std::chrono::milliseconds m_packetInterval;

	bool m_willSendPcktThisFrame;

	std::unordered_map<sockaddr_in, ClientInfo> m_clients;

	using SnapshotBufT = decltype(m_sendPacket.ghostSnapshotBuffer);

	void receiveAndProcess();

	void broadcastToClients(const Packet& packet);

	// Detects and handles entity destruction.
	// Clients will be notified of entity destruction via a
	// remote procedure call.
	void handleEntityDestruction(Entity&);

	// Selects 'maxSnapshots' entities from an array by their network priority
	// and places snapshots of them into the supplied snapshot buffer.
	// The buffer will be cleared before selecting snapshots into it.
	// Entities placed in the buffer will have their priorities reset to 0.
	void selectGhostSnapshots(SnapshotBufT& dst, std::vector<Entity*>& src,
	                          size_t maxSnapshots);
};

