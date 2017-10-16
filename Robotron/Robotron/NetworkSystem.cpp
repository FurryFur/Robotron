#include "NetworkSystem.h"

#include "InputComponent.h"
#include "Packet.h"

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

	// Server: Update inputs for entities

	// Client: Update ghost snapshots for ghost entities
}

void NetworkSystem::update(size_t entityID) 
{
	// Server: Send snapshot of entity to clients

	// Client: Do interpolation between current state and snapshot
}

void NetworkSystem::sendData()
{
	// A test packet to send
	Packet packet;

	static bool sendInput = true;
	if (sendInput) {
		InputComponent input;
		input.axis.x = 1.255;
		input.axis.y = 0.5;
		input.axis.z = 0.2;
		input.btn1Down = true;
		input.btn2Down = false;
		input.btn3Down = true;
		input.btn4Down = true;
		input.orientationDelta.x = 1.5;
		input.orientationDelta.y = 0.2;
		input.orientationDelta.z = 0.1;

		packet.type = PacketType::Input;
		packet.input = input;
	} else {
		packet.type = PacketType::GhostSnapshot;
		packet.ghostSnapshot = {};
		packet.ghostSnapshot.acceleration.z = 0.2548;
	}
	sendInput = !sendInput;

	// Fill out address to send to (including port)
	sockaddr_in address;
	inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
	address.sin_port = htons(4567);
	address.sin_family = AF_INET;

	// Send the packet
	int numBytesSent = sendto(
		m_socket.getSocketHandle(),                     // socket to send through.
		reinterpret_cast<const char*>(&packet),         // data to send
		sizeof(packet),                                 // number of bytes to send
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
	Packet receivedPacket;
	int numBytesRead = recvfrom(
		m_socket2.getSocketHandle(),
		reinterpret_cast<char*>(&receivedPacket),
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
		std::cout << "Received a packet from " << pcAddress << ":" << port << std::endl;
		if (receivedPacket.type == PacketType::Input) {
			std::cout << "Packet type is Input" << std::endl;
			std::cout << "Input x axis is: " << receivedPacket.input.axis.x << std::endl;
			std::cout << "Input btn4 down state is: " << receivedPacket.input.btn4Down << std::endl;
			std::cout << std::endl;
		} else {
			std::cout << "Packet type is Ghost Snapshot" << std::endl;
			std::cout << "Acceleration.z is: " << receivedPacket.ghostSnapshot.acceleration.z << std::endl;
			std::cout << std::endl;
		}

		// Cleanup
		delete[] pcAddress;
	}
}
