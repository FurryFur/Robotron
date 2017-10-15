#include "NetworkSystem.h"

#include <iostream>

NetworkSystem::NetworkSystem()
{
	// Startup windows sockets:
	WSADATA wsaData;
	int error;
	if (WSAStartup(0x0202, &wsaData) != 0)
	{
		error = WSAGetLastError();
		std::cout << "Windows Socket Error: " << error << std::endl;
	}

	m_socket.initialise(8456);
}

void NetworkSystem::beginFrame()
{
	//sendto(
	//	m_socket.GetSocketHandle(),				// socket to send through.
	//	dataToSend,									// data to send
	//	_iBytesToSend,									// number of bytes to send
	//	0,												// flags
	//	reinterpret_cast<const sockaddr*>(&address),	// address to be filled with packet target
	//	sizeof(address)							// size of the above address struct.
	//);
}