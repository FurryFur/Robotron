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
		return lhs->network.priority > rhs->network.priority;
	}
};

class NetworkServerSystem : public NetworkSystem {
public:
	NetworkServerSystem(Scene&);

	virtual void beginFrame() override;
	virtual void update(Entity&) override;
	virtual void endFrame() override;

private:
	// The time at which the last packet was sent
	std::chrono::time_point<std::chrono::high_resolution_clock> m_tLastPacketSent;

	// The interval between sending packets to clients
	std::chrono::milliseconds m_packetInterval;

	// The current sequence number for packets
	std::uint32_t m_curSeqenceNum;

	bool m_willSendPcktThisFrame;

	std::unordered_map<sockaddr_in, ClientInfo> m_clients;

	using PriorityQT = std::priority_queue<Entity*, std::vector<Entity*>, EntityPriorityComparitor>;
	using SnapshotBufT = decltype(m_sendPacket.ghostSnapshotBuffer);

	// A priority queue that sorts entities by their accumulated 
	// network priority. This queue is used to decide which entities
	// will have snapshots sent out when a packet is sent out to clients
	// on the network.
	PriorityQT m_snapshotPriorityQ;

	void receiveAndProcess();

	void broadcastToClients(const Packet& packet);

	// Detects and handles entity destruction.
	// Clients will be notified of entity destruction via a
	// remote procedure call.
	void handleEntityDestruction(Entity&);

	// Saves RPC calls to be sent to clients when the next update packet is 
	// sent out.
	void bufferRpc(std::unique_ptr<RemoteProcedureCall> rpc);

	// Pulls 'maxSnapshots' entities off a priority queue and adds  
	// snapshots of them to a buffer.
	// The buffer will be cleared before selecting snapshots into it.
	// Entities placed in the buffer will have their priorities reset to 0.
	void selectGhostSnapshots(SnapshotBufT& dst, PriorityQT& src, 
	                          size_t maxSnapshots);
};

