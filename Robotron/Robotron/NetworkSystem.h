#pragma once

#include "socket.h"
#include "BufferStream.h"
#include "Packet.h"

#include <vector>

struct Scene;
struct Packet;
struct Entity;

class NetworkSystem {
public:
	NetworkSystem(Scene&);

	virtual void beginFrame() = 0;
	virtual void update(Entity&) = 0;
	virtual void endFrame() = 0;

protected:
	void sendData(const Packet&, const sockaddr_in&);
	bool receiveData(Packet& outPacket, sockaddr_in& outAddress);
	void allocateRecvBuffer();

	CSocket m_socket;
	Scene& m_scene;
	std::vector<Entity*> m_netEntities;
	std::vector<char> m_recvBuffer;

	// A reusable packet for sending data
	Packet m_sendPacket;

	// A reusable packet for receiving data
	Packet m_recvPacket;

	OutBufferStream m_obs;
};