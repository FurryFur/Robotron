#include "NetworkSystem.h"

#include <iostream>
#include <memory>

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
	m_socket2.initialise(4567);
}

void NetworkSystem::beginFrame()
{
	sendData();
	receiveData();
}

void NetworkSystem::sendData()
{
	// A test packet to send
	char testPacket = 'a';

	// Fill out address to send to (including port)
	sockaddr_in address;
	inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
	address.sin_port = htons(4567);
	address.sin_family = AF_INET;

	// Send the packet
	int numBytesSent = sendto(
		m_socket.getSocketHandle(),                     // socket to send through.
		&testPacket,                                    // data to send
		sizeof(testPacket),                             // number of bytes to send
		0,                                              // flags
		reinterpret_cast<const sockaddr*>(&address),    // address to be filled with packet target
		sizeof(address)                                 // size of the above address struct.
	);

	// Do error checking
	int error = WSAGetLastError();
	if (error != 0)
		std::cout << "Error sending data, error code: " << error << std::endl;
}

void NetworkSystem::receiveData()
{
	sockaddr_in fromAddress;
	int sizeofAddress = sizeof(fromAddress);

	// Retrieve a dataframe from the socket
	char receivedPacket;
	int numBytesRead = recvfrom(
		m_socket2.getSocketHandle(),
		&receivedPacket,
		sizeof(receivedPacket),
		0,
		reinterpret_cast<sockaddr*>(&fromAddress),
		&sizeofAddress
	);

	// Do error checking
	int error = WSAGetLastError();
	if (error != 0 && error != WSAEWOULDBLOCK)
		std::cout << "Error receiving data, error code: " << error << std::endl;

	// Do stuff with result
	if (numBytesRead > 0) {
		// Create a human readable version of the address
		char* pcAddress = new char[sizeofAddress];
		inet_ntop(AF_INET, &fromAddress, pcAddress, sizeofAddress);
		u_short port = ntohs(fromAddress.sin_port);
		std::cout << "Received " << receivedPacket << " from " << pcAddress << ":" << port << std::endl;

		// Cleanup
		delete[] pcAddress;
	}
}
