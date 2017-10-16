#pragma once

#include "socket.h"

class NetworkSystem {
public:
	NetworkSystem();

	void beginFrame();

private:
	void sendData();
	void receiveData();

	CSocket m_socket;
	CSocket m_socket2; // Temporary for testing
};