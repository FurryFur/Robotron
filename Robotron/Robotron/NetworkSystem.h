#pragma once

#include "socket.h"

class NetworkSystem {
public:
	NetworkSystem();

	void beginFrame();

private:
	CSocket m_socket;
};