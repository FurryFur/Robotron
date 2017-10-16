#pragma once

#include "socket.h"

class NetworkSystem {
public:
	NetworkSystem();

	virtual void beginFrame(); // = 0
	virtual void update(size_t entityID); // = 0

private:
	void sendData();
	void receiveData();

	CSocket m_socket;
	CSocket m_socket2; // Temporary for testing
};