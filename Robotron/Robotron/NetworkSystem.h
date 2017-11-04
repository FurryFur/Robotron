#pragma once

#include "socket.h"
#include "BufferStream.h"
#include "Packet.h"

#include <vector>
#include <chrono>

class Scene;
class Entity;
class RemoteProcedureCall;
class LobbyEventListener;
struct Packet;

class NetworkSystem {
public:
	NetworkSystem(Scene&);

	virtual void beginFrame() = 0;
	virtual void update(Entity&, float deltaTick) = 0;
	virtual void endFrame() = 0;
	virtual bool isInGame() = 0;
	virtual void startGame() = 0;

	void setLobbyEventListener(LobbyEventListener*);

private:
	std::vector<char> m_recvBuffer;

protected:
	// Send a packet to the specified address
	void sendData(const Packet&, const sockaddr_in& address);

	// Receive a packet and sender address into the specified out parameters
	bool receiveData(Packet& outPacket, sockaddr_in& outAddress);

	// Allocates the buffer for receiving data on the socket.
	void allocateRecvBuffer();

	// Saves RPC calls to be sent to clients when the next update packet is 
	// sent out.
	void bufferRpc(std::unique_ptr<RemoteProcedureCall> rpc);

	CSocket m_socket;
	Scene& m_scene;
	std::vector<Entity*> m_netEntities;
	LobbyEventListener* m_lobbyEventListener;

	// Determines whether the implementing client/server will send out a packet
	// this frame or not
	bool m_willSendPcktThisFrame;

	// The time at which the last packet was sent
	std::chrono::time_point<std::chrono::high_resolution_clock> m_tLastPacketSent;

	// The interval between sending packets to clients
	std::chrono::milliseconds m_packetInterval;

	// The current sequence number for sending packets
	std::uint32_t m_curSeqenceNum;

	// A reusable packet for sending data
	Packet m_sendPacket;

	// A reusable packet for receiving data
	Packet m_recvPacket;

	OutBufferStream m_obs;
};