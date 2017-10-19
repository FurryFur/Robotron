#pragma once

#include "socket.h"

#include <vector>

struct Scene;
struct Packet;
struct Entity;

class NetworkSystem {
public:
	NetworkSystem(Scene&);

	virtual void beginFrame() = 0;
	virtual void update(Entity&) = 0;

protected:
	void sendData(const Packet&, const sockaddr_in&);
	bool receiveData(Packet&);

	CSocket m_socket;
	Scene& m_scene;
	std::vector<Entity*> m_netEntities;
};