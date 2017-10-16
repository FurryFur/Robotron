#include "NetworkSystem.h"

#include "InputComponent.h"
#include "Packet.h"
#include "Scene.h"

#include <iostream>
#include <memory>

NetworkSystem::NetworkSystem(Scene& scene)
	: m_scene{ scene }
{
	// Startup windows sockets:
	WSADATA wsaData;
	int error;
	if (WSAStartup(0x0202, &wsaData) != 0)
	{
		error = WSAGetLastError();
		std::cout << "Windows Socket Error: " << error << std::endl;
	}
}

void NetworkSystem::beginFrame()
{
	// Server: Update inputs for entities

	// Client: Update ghost snapshots for ghost entities
}

void NetworkSystem::update(size_t entityID) 
{
	// Server: Send snapshot of entity to clients

	// Client: Do interpolation between current state and snapshot
}

void NetworkSystem::sendData(const Packet& packet, const sockaddr_in& address)
{
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
	if (error != 0) {
		// TODO: Log these error messages properly
		std::cout << "Error sending data, error code: " << error << std::endl;
	}
}

bool NetworkSystem::receiveData(Packet& outPacket)
{
	sockaddr_in fromAddress;
	int sizeofAddress = sizeof(fromAddress);

	// Retrieve a dataframe from the socket
	int numBytesRead = recvfrom(
		m_socket.getSocketHandle(),
		reinterpret_cast<char*>(&outPacket),
		sizeof(outPacket),
		0,
		reinterpret_cast<sockaddr*>(&fromAddress),
		&sizeofAddress
	);

	// Do error checking
	int error = WSAGetLastError();
	if (error == WSAEWOULDBLOCK)
		return false;
	if (error != 0 && error != WSAEWOULDBLOCK) {
		std::cout << "Error receiving data, error code: " << error << std::endl;
		return false;
	}

	// Do stuff with result
	if (numBytesRead > 0) {
		// Create a human readable version of the address
		char* pcAddress = new char[sizeofAddress];
		inet_ntop(AF_INET, &fromAddress, pcAddress, sizeofAddress);
		u_short port = ntohs(fromAddress.sin_port);
		std::cout << "Received a packet from " << pcAddress << ":" << port << std::endl;
		if (outPacket.type == PacketType::PACKET_TYPE_INPUT) {
			std::cout << "Packet type is Input" << std::endl;
			std::cout << "Input x axis is: " << outPacket.input.axis.x << std::endl;
			std::cout << "Input btn4 down state is: " << outPacket.input.btn4Down << std::endl;
			std::cout << std::endl;
		} else if (outPacket.type == PACKET_TYPE_GHOST_SNAPSHOT) {
			std::cout << "Packet type is Ghost Snapshot" << std::endl;
			std::cout << "Acceleration.z is: " << outPacket.ghostSnapshot.acceleration.z << std::endl;
			std::cout << std::endl;
		} else {
			std::cout << "Packet type is Transform" << std::endl;
			std::cout << "X position is: " << outPacket.transform[3].x << std::endl;
			std::cout << "Y position is: " << outPacket.transform[3].y << std::endl;
			std::cout << "Z position is: " << outPacket.transform[3].z << std::endl;
		}

		// Cleanup
		delete[] pcAddress;

		return true;
	}

	return false;
}
