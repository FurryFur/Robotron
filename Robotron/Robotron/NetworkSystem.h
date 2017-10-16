#pragma once

#include "socket.h"

struct Scene;
struct Packet;

class NetworkSystem {
public:
	NetworkSystem(Scene&);

	virtual void beginFrame() = 0;
	virtual void update(size_t entityID) = 0;

protected:
	void sendData(const Packet&, const sockaddr_in&);
	bool receiveData(Packet&);

	CSocket m_socket;
	//CSocket m_socket2; // TODO: Remove temporary receive socket
	Scene& m_scene;
};