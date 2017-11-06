#include "NetworkSystem.h"

#include "socket.h"
#include "InputComponent.h"
#include "Packet.h"
#include "Scene.h"
#include "RPC.h"
#include "NetworkEventListener.h"

#include "Log.h"
#include <memory>
#include <cstdint>

using namespace std::chrono;
using namespace std::chrono_literals;

const milliseconds NetworkSystem::s_kKeepAliveTimout = 10000ms;

NetworkSystem::NetworkSystem(Scene& scene)
	: m_scene{ scene }
	, m_curSeqenceNum{ 1 }
	, m_packetInterval{ 25ms }
{
	// Startup windows sockets:
	WSADATA wsaData;
	int error;
	if (WSAStartup(0x0202, &wsaData) != 0)
	{
		error = WSAGetLastError();
		g_out << "Windows Socket Error: " << error << "\n";
	}

	m_sendPacket.rpcGroupBuffer.emplace();
}

NetworkSystem::~NetworkSystem()
{
}

void NetworkSystem::beginFrame()
{
	// Decide whether we will send out packets this frame or not
	auto now = high_resolution_clock::now();
	auto dtLastPacketSent = now - m_tLastPacketSent;
	if (dtLastPacketSent >= m_packetInterval) {
		m_willSendPcktThisFrame = true;
		m_tLastPacketSent = now;
	} else {
		m_willSendPcktThisFrame = false;
	}
}

void NetworkSystem::endFrame()
{
	if (m_willSendPcktThisFrame) {
		++m_curSeqenceNum;
		// New RPC group for new sequence number
		m_sendPacket.rpcGroupBuffer.emplace();
	}
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
		static_cast<int>(m_obs.getBytesWritten()),      // number of bytes to send
		0,                                              // flags
		reinterpret_cast<const sockaddr*>(&address),    // address to be filled with packet target
		sizeof(address)                                 // size of the above address struct.
	);

	// Do error checking
	int error = WSAGetLastError();
	if (error != 0) {
		// TODO: Log these error messages properly
		g_out << "Error sending data, error code: " << error << "\n";
	}
}

bool NetworkSystem::receiveData(Packet& outPacket, sockaddr_in& outAddress)
{
	int sizeofAddress = sizeof(outAddress);

	// Retrieve a datagram from the socket
	int numBytesRead = recvfrom(
		m_socket.getSocketHandle(),
		m_recvBuffer.data(),
		static_cast<int>(m_recvBuffer.size()),
		0,
		reinterpret_cast<sockaddr*>(&outAddress),
		&sizeofAddress
	);

	// Do error checking
	int error = WSAGetLastError();
	if (error == WSAEWOULDBLOCK)
		return false;
	if (error != 0 && error != WSAEWOULDBLOCK) {
		g_out << "Error receiving data, error code: " << error << "\n";
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
		g_out << "Warning: Unable to get maximum datagram size\n";
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

void NetworkSystem::registerEntityEventListener(NetworkEventListener* eventListener)
{
	if (eventListener)
		m_eventListeners.push_back(eventListener);
	else {
		// TODO: Add logging here
		g_out << "WARNING: Tried to register nullptr as a Network Event Listener\n";
	}
}

void NetworkSystem::removeEntityEventListener(NetworkEventListener* eventListener)
{
	auto removeIt = std::remove(m_eventListeners.begin(), m_eventListeners.end(), eventListener);
	if (removeIt != m_eventListeners.end())
		m_eventListeners.erase(removeIt);
	else {
		// TODO: Add logging here
		g_out << "WARNING: Tried to remove a Network Event Listener that wasn't registered with the network system\n";
	}
}
