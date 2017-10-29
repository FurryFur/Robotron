#pragma once

#include "socket.h"
#include "BufferStream.h"
#include "Packet.h"

#include <vector>

class Scene;
class Entity;
class RemoteProcedureCall;
struct Packet;

class NetworkSystem {
public:
	NetworkSystem(Scene&);

	virtual void beginFrame() = 0;
	virtual void update(Entity&, float deltaTick) = 0;
	virtual void endFrame() = 0;

protected:
	void sendData(const Packet&, const sockaddr_in&);
	bool receiveData(Packet& outPacket, sockaddr_in& outAddress);
	void allocateRecvBuffer();

	// Saves RPC calls to be sent to clients when the next update packet is 
	// sent out.
	void bufferRpc(std::unique_ptr<RemoteProcedureCall> rpc);

	CSocket m_socket;
	Scene& m_scene;
	std::vector<Entity*> m_netEntities;
	std::vector<char> m_recvBuffer;

	// The current sequence number for sending packets
	std::uint32_t m_curSeqenceNum;

	// A reusable packet for sending data
	Packet m_sendPacket;

	// A reusable packet for receiving data
	Packet m_recvPacket;

	OutBufferStream m_obs;
};