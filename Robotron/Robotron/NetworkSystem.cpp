#include "NetworkSystem.h"

#include "InputComponent.h"
#include "Packet.h"
#include "Scene.h"
#include "RPC.h"

#include <iostream>
#include <memory>
#include <cstdint>

NetworkSystem::NetworkSystem(Scene& scene)
	: m_scene{ scene }
	, m_curSeqenceNum{ 0 }
{
	// Startup windows sockets:
	WSADATA wsaData;
	int error;
	if (WSAStartup(0x0202, &wsaData) != 0)
	{
		error = WSAGetLastError();
		std::cout << "Windows Socket Error: " << error << std::endl;
	}

	m_sendPacket.rpcGroupBuffer.emplace();
}

void NetworkSystem::endFrame()
{
	++m_curSeqenceNum;
	// New RPC group for new sequence number
	m_sendPacket.rpcGroupBuffer.emplace();
}

void NetworkSystem::sendData(const Packet& packet, const sockaddr_in& address)
{
	m_obs.reset();
	m_obs << packet;
	const char* packetData = m_obs.getData();

	// Send the packet
	// TODO: Only send the number of bytes required for the packet type.
	int numBytesSent = sendto(
		m_socket.getSocketHandle(),                     // socket to send through.
		packetData,                                     // data to send
		m_obs.getBytesWritten(),                        // number of bytes to send
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

bool NetworkSystem::receiveData(Packet& outPacket, sockaddr_in& outAddress)
{
	int sizeofAddress = sizeof(outAddress);

	// Retrieve a datagram from the socket
	int numBytesRead = recvfrom(
		m_socket.getSocketHandle(),
		m_recvBuffer.data(),
		m_recvBuffer.size(),
		0,
		reinterpret_cast<sockaddr*>(&outAddress),
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

	if (numBytesRead > 0) {
		InBufferStream ibs(m_recvBuffer);
		ibs >> outPacket;

		if (!ibs.getError()) {
			return true;
		}
	}

	return false;
}

void NetworkSystem::allocateRecvBuffer()
{
	// Determine to maximum size required for the receive buffer
	int maxBufferSize;
	int maxBufferSizeSize = sizeof(maxBufferSize);
	int result = getsockopt(m_socket.getSocketHandle(), SOL_SOCKET, SO_MAX_MSG_SIZE, (char *)&maxBufferSize, &maxBufferSizeSize);
	if (result == SOCKET_ERROR) {
		// TODO: Add logging here
		std::cout << "Warning: Unable to get maximum datagram size" << std::endl;
		m_recvBuffer.resize(64000);
	} else {
		m_recvBuffer.resize(maxBufferSize);
	}
}

void NetworkSystem::bufferRpc(std::unique_ptr<RemoteProcedureCall> rpc)
{
	// Add the rpc to the latest RPCGroup
	RPCGroup& rpcGroup = m_sendPacket.rpcGroupBuffer.front();
	rpcGroup.addRPC(std::move(rpc));
}
