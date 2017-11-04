#include "NetworkClientSystem.h"

#include "Scene.h"
#include "Packet.h"
#include "EntityUtils.h"
#include "Entity.h"
#include "GhostSnapshot.h"
#include "RPC.h"
#include "NetworkServerSystem.h"
#include "LobbyEventListener.h"

#include <glm\gtx\matrix_interpolation.hpp>
#include <glm\gtx\compatibility.hpp>
#include <glm\gtx\matrix_interpolation.hpp>

#include <cmath>
#include <iostream>
#include <cstdint>

NetworkClientSystem::NetworkClientSystem(Scene& scene, const std::string& username)
	: NetworkSystem(scene)
	, m_lastSeqNumSeen{ 0 }
	, m_clientState{ CLIENT_STATE_NO_SERVER }
	, m_username{ username }
{
	m_socket.initialise(4567);
	allocateRecvBuffer();
	RPC::setClient(this);

	// TODO: Add broadcast to find servers
	//inet_pton(AF_INET, "127.0.0.1", &m_serverAddress.sin_addr);
	//m_serverAddress.sin_port = htons(8456);
	//m_serverAddress.sin_family = AF_INET;
}

void NetworkClientSystem::beginFrame()
{
	sockaddr_in address;
	while (receiveData(m_recvPacket, address)) {
		switch (m_clientState)
		{
		case CLIENT_STATE_NO_SERVER:
			handlePreLobbyPackets(m_recvPacket, address);
			break;
		case CLIENT_STATE_IN_LOBBY:
		case CLIENT_STATE_IN_GAME:
			handleGamePackets(m_recvPacket, address);
			break;
		default:
			break;
		}
	}

	if (m_clientState != CLIENT_STATE_NO_SERVER)
		NetworkSystem::beginFrame();
}

void NetworkClientSystem::update(Entity& entity, float deltaTick)
{
	if (m_clientState != CLIENT_STATE_IN_GAME)
		return;

	if (!entity.hasComponents(COMPONENT_NETWORK))
		return;

	std::int32_t id = entity.network.id;

	// TODO: Add logging here
	if (id < 0) {
		//std::cout << "Error: Entity with unassigned network id seen by client" << std::endl;
		return;
	}

	// Send input updates to server
	if (entity.hasComponents(COMPONENT_INPUT)) {
		bufferRpc(std::make_unique<RPCRecordInput>(id, entity.input));
	}
}

void NetworkClientSystem::endFrame()
{
	if (m_clientState != CLIENT_STATE_NO_SERVER) {
		if (m_willSendPcktThisFrame) {
			m_sendPacket.packetType = PACKET_TYPE_NORMAL;
			m_sendPacket.sequenceNum = m_curSeqenceNum;

			sendData(m_sendPacket, m_serverAddress);
		}

		NetworkSystem::endFrame();
	}
}

bool NetworkClientSystem::isInGame()
{
	return m_clientState == CLIENT_STATE_IN_GAME;
}

void NetworkClientSystem::broadcastForServers()
{
	m_clientState = CLIENT_STATE_NO_SERVER;
	
	Packet broadcastPacket;
	broadcastPacket.packetType = PACKET_TYPE_BROADCAST;

	m_socket.enableBroadcast();
	m_socket.setRemoteAddress(INADDR_BROADCAST);
	m_socket.setRemotePort(NetworkServerSystem::s_kDefaultServerPort);

	sockaddr_in address;
	inet_pton(AF_INET, "255.255.255.255", &address.sin_addr);
	address.sin_family = AF_INET;
	address.sin_addr.S_un.S_addr = INADDR_BROADCAST;

	for (int i = 0; i < 10; i++) //Just try  a series of 10 ports to detect a runmning server; this is needed since we are testing multiple servers on the same local machine
	{
		address.sin_port = htons(NetworkServerSystem::s_kDefaultServerPort + i);
		// TODO: Add logging here
		std::cout << "INFO: Sending broadcast packet to address: " << toString(address) << std::endl;
		sendData(broadcastPacket, address);
	}

	m_socket.disableBroadcast();
}

void NetworkClientSystem::joinServer(const sockaddr_in& address)
{
	m_serverAddress = address;

	Packet joinResq;
	joinResq.packetType = PACKET_TYPE_JOIN_REQUEST;
	joinResq.username = m_username;
	sendData(joinResq, address);
}

void NetworkClientSystem::updatePlayers(const std::vector<PlayerInfo>& currentPlayers)
{
	// TODO: Add logging here
	std::cout << "Received a lobby update, current players:" << std::endl;
	for (auto& playerInfo : currentPlayers) {
		std::cout << playerInfo.username << std::endl;
	}

	if (m_clientState == CLIENT_STATE_IN_LOBBY && m_lobbyEventListener)
		m_lobbyEventListener->handleLobbyUpdate(currentPlayers);
}

void NetworkClientSystem::createGhost(std::int32_t entityNetId, ModelID modelId, const TransformComponent& transform)
{
	// Destroy existing entities with the same id before creating new ones
	// TODO: Add logging here
	if (destroyIfExistsInNetwork(entityNetId))
		std::cout << "INFO: Overwritting entity with network id: " << entityNetId << std::endl;

	Entity& newEntity = EntityUtils::createGhost(m_scene, modelId, transform, entityNetId);

	// Add entity to network system tracking
	if (entityNetId >= m_netEntities.size())
		m_netEntities.resize(entityNetId + 1);
	m_netEntities.at(entityNetId) = &newEntity;
}

void NetworkClientSystem::createPlayerGhost(std::int32_t entityNetId, const PlayerInfo& playerInfo, const TransformComponent& transform)
{
	// Destroy existing entities with the same id before creating new ones
	// TODO: Add logging here
	if (destroyIfExistsInNetwork(entityNetId))
		std::cout << "INFO: Overwritting entity with network id: " << entityNetId << std::endl;

	Entity& newEntity = EntityUtils::createPlayerGhost(m_scene, playerInfo, transform, entityNetId);

	// TODO: Conditionally add the player controller if the username matches the clients username
	// newEntity.addComponents(COMPONENT_PLAYER_CONTROL);
	newEntity.addComponents(COMPONENT_INPUT_MAP);
	newEntity.inputMap.mouseInputEnabled = false;
	newEntity.inputMap.leftBtnMap = GLFW_KEY_A;
	newEntity.inputMap.rightBtnMap = GLFW_KEY_D;
	newEntity.inputMap.forwardBtnMap = GLFW_KEY_W;
	newEntity.inputMap.backwardBtnMap = GLFW_KEY_S;

	// Add entity to network system tracking
	if (entityNetId >= m_netEntities.size())
		m_netEntities.resize(entityNetId + 1);
	m_netEntities.at(entityNetId) = &newEntity;
}

void NetworkClientSystem::handlePreLobbyPackets(const Packet& packet, const sockaddr_in& address)
{
	switch (packet.packetType)
	{
	case PACKET_TYPE_NORMAL:
		handleGamePackets(packet, address);
	case PACKET_TYPE_BROADCAST_RESPONSE:
		// If we receive a broadcast response from a server in this mode, simple inform the 
		// lobby event listener.
		if (m_lobbyEventListener)
			m_lobbyEventListener->handleBroadcastResponse(packet.serverName, address);

		std::cout << "Received broadcast response from server: " 
		          << packet.serverName << ", at address: " << toString(address)
		          << std::endl;

		// TODO: Remove auto join, replace with lobby
		//std::cout << "Attempting to auto join server: " << packet.serverName 
		//          << std::endl;
		//joinServer(address);

		break;
	case PACKET_TYPE_JOIN_RESPONSE:
		if (packet.joinAccepted) {
			// TODO: Change this back to CLIENT_STATE_IN_LOBBY once lobby is working
			// m_clientState = CLIENT_STATE_IN_LOBBY;
			// For now just jump straight into the game
			m_clientState = CLIENT_STATE_IN_GAME;

			std::cout << "Received join accept from server at address: " 
			          << toString(address) << std::endl;
			if (m_lobbyEventListener)
				m_lobbyEventListener->handleJoinAccepted();
		} else {
			m_clientState = CLIENT_STATE_NO_SERVER;

			std::cout << "Received join reject from server at address: " 
			          << toString(address) << std::endl;
			if (m_lobbyEventListener)
				m_lobbyEventListener->handleJoinRejected();
		}
		break;
	default:
		break;
	}
}

void NetworkClientSystem::handleGamePackets(const Packet& packet, const sockaddr_in& address)
{
	if (address != m_serverAddress) {
		// TODO: Add logging here
		std::cout << "INFO: Received packet from unknown host" << std::endl;
		return;
	}

	auto& ghostSnapshots = packet.ghostSnapshotBuffer;
	auto& rpcGroups = packet.rpcGroupBuffer;

	// Get figure out which remote procedure calls we got from the server
	// that we haven't seen before.
	std::uint32_t seqNumRecvd = packet.sequenceNum;
	std::uint32_t bufferOffset = std::min((seqNumRecvd - m_lastSeqNumSeen - 1),
		(static_cast<std::uint32_t>(rpcGroups.size() - 1)));

	// Execute RPCs received from server
	for (int i = bufferOffset; i >= 0; --i) {
		const RPCGroup& rpcGroup = rpcGroups.at(i);
		for (auto& rpc : rpcGroup.getRpcs()) {
			rpc->execute();
		}
	}

	if (seqNumRecvd > m_lastSeqNumSeen) {
		// Save ghost snapshots when new data received
		for (std::uint16_t i = 0; i < ghostSnapshots.size(); ++i) {
			const GhostSnapshot& ghostSnapshot = ghostSnapshots.at(i);
			std::int32_t id = ghostSnapshot.entityNetId;

			// TODO: Add logging here
			if (id < 0) {
				std::cout << "Error: Ghost Snapshot with unassigned network id received by client" << std::endl;
				return;
			}

			// Record ghost snapshot
			if (id < m_netEntities.size()) {
				Entity* entity = m_netEntities.at(id);
				if (entity) {
					entity->transform = ghostSnapshot.transform;
					entity->physics = ghostSnapshot.physics;
				}
				else {
					// TODO: Add logging here
					std::cout << "Warning: Client received snapshot of deleted ghost" << std::endl;
				}
			}
		}

		// Update the last sequence number seen from the server
		m_lastSeqNumSeen = seqNumRecvd;
	}
}

bool NetworkClientSystem::destroyIfExistsInNetwork(std::int32_t entityNetId)
{
	if (0 <= entityNetId && entityNetId < m_netEntities.size()) {
		Entity* existingEntity = m_netEntities.at(entityNetId);
		if (existingEntity) {
			existingEntity->destroy();
			m_netEntities.at(entityNetId) = nullptr;
			return true;
		}
	}

	return false;
}
