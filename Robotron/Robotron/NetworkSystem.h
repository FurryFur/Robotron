#pragma once

#include "socket.h"
#include "BufferStream.h"

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

	CSocket m_socket;
	Scene& m_scene;
	std::vector<Entity*> m_netEntities;
	OutBufferStream m_obs;
};