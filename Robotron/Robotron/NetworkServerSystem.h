#pragma once

#include "NetworkSystem.h"
#include "socket.h"
#include "RPC.h"
#include "StreamableQueue.h"

#include <vector>
#include <unordered_set>
#include <list>
#include <memory>

struct Scene;
struct Entity;

class NetworkServerSystem : public NetworkSystem {
public:
	NetworkServerSystem(Scene&);

	virtual void beginFrame() override;
	virtual void update(Entity&) override;
	virtual void endFrame() override;

private:
	void broadcastToClients(Packet packet);
	void destroyNetworkEntity(std::int32_t id);
	void bufferRpc(std::unique_ptr<RemoteProcedureCall> rpc);

	StreamableQueue<RPCGroup, 32> m_rpcGroupBuffer;
	std::uint32_t m_curSeqenceNum;
	std::unordered_set<sockaddr_in> m_clients;
};

