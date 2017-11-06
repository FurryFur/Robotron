#include "NetworkClientSystem.h"

#include "Scene.h"
#include "Packet.h"
#include "EntityUtils.h"
#include "Entity.h"
#include "GhostSnapshot.h"
#include "RPC.h"
#include "NetworkServerSystem.h"
#include "NetworkEventListener.h"

#include <glm\gtx\matrix_interpolation.hpp>
#include <glm\gtx\compatibility.hpp>
#include <glm\gtx\matrix_interpolation.hpp>

#include <cmath>
#include "Log.h"
#include <cstdint>
#include <chrono>

using namespace std::chrono;
using namespace std::chrono_literals;

NetworkClientSystem::NetworkClientSystem(Scene& scene, Audio& audioSystem, const std::string& username)
	: NetworkSystem(scene)
	, m_lastSeqNumSeen{ 0 }
	, m_clientState{ CLIENT_STATE_NO_SERVER }
	, m_username{ username }
	, m_audioSystem{ audioSystem }
{
	m_socket.initialise(4567);
	allocateRecvBuffer();
	RPC::setClient(this);

	// TODO: Add broadcast to find servers
	//inet_pton(AF_INET, "127.0.0.1", &m_serverAddress.sin_addr);
	//m_serverAddress.sin_port = htons(8456);
	//m_serverAddress.sin_family = AF_INET;
}

NetworkClientSystem::~NetworkClientSystem()
{
}

void NetworkClientSystem::beginFrame()
{
	sockaddr_in address;
	while (receiveData(m_recvPacket, address)) {
		// Keep alive update
		m_tLastPacketRecvd = high_resolution_clock::now();

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
	
	// Check keep alive
	if (m_clientState != CLIENT_STATE_NO_SERVER) {
		auto now = high_resolution_clock::now();
		auto timeSinceLastPacket = now - m_tLastPacketRecvd;
		if (timeSinceLastPacket > NetworkSystem::s_kKeepAliveTimout) {
			m_clientState = CLIENT_STATE_NO_SERVER;
			if (m_eventListeners.size() > 0) {
				for (auto eventListener : m_eventListeners)
					eventListener->onDisconnect();
			}
		}
	}

	// Check if we will send a packet this frame
	NetworkSystem::beginFrame();

	// Don't send packets if we have noone to send to
	if (m_clientState == CLIENT_STATE_NO_SERVER)
		m_willSendPcktThisFrame = false;
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
		//g_out << "Error: Entity with unassigned network id seen by client\n";
		return;
	}

	// Send input updates to server
	if (entity.hasComponents(COMPONENT_INPUT_MAP, COMPONENT_INPUT)) {
		bufferRpc(std::make_unique<RPCRecordInput>(id, entity.input));
	}
}

void NetworkClientSystem::endFrame()
{
	if (playersToSpawn.size() > 0)
		spawnPlayers();

	if (m_willSendPcktThisFrame) {
		m_sendPacket.packetType = PACKET_TYPE_NORMAL;
		m_sendPacket.sequenceNum = m_curSeqenceNum;

		sendData(m_sendPacket, m_serverAddress);
	}

	NetworkSystem::endFrame();
}

bool NetworkClientSystem::isInGame()
{
	return m_clientState == CLIENT_STATE_IN_GAME;
}

void NetworkClientSystem::startGame()
{
	m_clientState = CLIENT_STATE_IN_GAME;

	for (auto eventListener : m_eventListeners)
		eventListener->onGameStart();
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
		g_out << "INFO: Sending broadcast packet to address: " << toString(address) << "\n";
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
	//g_out << "Received a lobby update, current players:\n";
	//for (auto& playerInfo : currentPlayers) {
	//	g_out << playerInfo.username << "\n";
	//}

	if (m_eventListeners.size() > 0)
	{
		for (auto eventListener : m_eventListeners)
			eventListener->onPlayersUpdated(currentPlayers);
	}
}

void NetworkClientSystem::playAudio(Sound sound)
{
	m_audioSystem.playSFX(sound);
}

void NetworkClientSystem::createGhost(std::int32_t entityNetId, ModelID modelId)
{
	// Destroy existing entities with the same id before creating new ones
	// TODO: Add logging here
	if (destroyIfExistsInNetwork(entityNetId))
		g_out << "INFO: Overwritting entity with network id: " << entityNetId << "\n";

	Entity& newEntity = EntityUtils::createGhost(m_scene, modelId, entityNetId);

	// Add entity to network system tracking
	if (entityNetId >= m_netEntities.size())
		m_netEntities.resize(entityNetId + 1);
	m_netEntities.at(entityNetId) = &newEntity;
}

void NetworkClientSystem::createPlayerGhost(std::int32_t entityNetId, const PlayerInfo& playerInfo)
{
	playersToSpawn.push_back(std::make_pair(entityNetId, playerInfo));
}

void NetworkClientSystem::spawnPlayers()
{
	for (auto& netIdPlayerInfoPair : playersToSpawn) {
		std::int32_t entityNetId = netIdPlayerInfoPair.first;
		PlayerInfo& playerInfo = netIdPlayerInfoPair.second;

		// Destroy existing entities with the same id before creating new ones
		// TODO: Add logging here
		if (destroyIfExistsInNetwork(entityNetId))
			g_out << "INFO: Overwritting entity with network id: " << entityNetId << "\n";

		Entity& newPlayer = EntityUtils::createPlayerGhost(m_scene, playerInfo, entityNetId);

		// Conditionally add the player controller if the username matches the clients username
		if (newPlayer.player.playerInfo.getPlayerID() == m_clientPlayerID) {
			newPlayer.addComponents(COMPONENT_INPUT_MAP, COMPONENT_INPUT);
			newPlayer.inputMap.mouseInputEnabled = false;
			newPlayer.inputMap.leftBtnMap = GLFW_KEY_A;
			newPlayer.inputMap.rightBtnMap = GLFW_KEY_D;
			newPlayer.inputMap.forwardBtnMap = GLFW_KEY_W;
			newPlayer.inputMap.backwardBtnMap = GLFW_KEY_S;

			// Give the local player a spotlight
			newPlayer.addComponents(COMPONENT_SPOTLIGHT);
			newPlayer.spotlight.direction = glm::vec3(0, -0.1, -1);
			newPlayer.spotlight.color = glm::vec3(0.5, 0.75, 1.5) * 4.0f;
		}

		// Add entity to network system tracking
		if (entityNetId >= m_netEntities.size())
			m_netEntities.resize(entityNetId + 1);
		m_netEntities.at(entityNetId) = &newPlayer;
	}

	playersToSpawn.clear();
}

void NetworkClientSystem::handlePreLobbyPackets(const Packet& packet, const sockaddr_in& address)
{
	switch (packet.packetType)
	{
	case PACKET_TYPE_NORMAL:
		handleGamePackets(packet, address);
		break;
	case PACKET_TYPE_BROADCAST_RESPONSE:
		// If we receive a broadcast response from a server in this mode, simple inform the 
		// lobby event listener.
		if (m_eventListeners.size() > 0)
		{
			for (auto eventListener : m_eventListeners)
				eventListener->onBroadcastResponse(packet.serverName, address);
		}

		g_out << "Received broadcast response from server: " 
		          << packet.serverName << ", at address: " << toString(address)
		          << "\n";

		// TODO: Remove auto join, replace with lobby
		//g_out << "Attempting to auto join server: " << packet.serverName 
		//          << "\n";
		//joinServer(address);

		break;
	case PACKET_TYPE_JOIN_RESPONSE:
		if (packet.joinAccepted) {
			m_clientState = CLIENT_STATE_IN_LOBBY;
			m_clientPlayerID = packet.playerID;

			g_out << "Received join accept from server at address: " 
			          << toString(address) << "\n";
			if (m_eventListeners.size() > 0)
			{
				for (auto eventListener : m_eventListeners)
					eventListener->onJoinAccepted();
			}
		} else {
			m_clientState = CLIENT_STATE_NO_SERVER;

			g_out << "Received join reject from server at address: " 
			          << toString(address) << "\n";
			if (m_eventListeners.size() > 0)
			{
				for (auto eventListener : m_eventListeners)
					eventListener->onJoinRejected();
			}
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
		g_out << "INFO: Received packet from unknown host\n";
		return;
	}

	auto& ghostSnapshots = packet.ghostSnapshotBuffer;
	auto& rpcGroups = packet.rpcGroupBuffer;

	// Get figure out which remote procedure calls we got from the server
	// that we haven't seen before.
	std::uint32_t seqNumRecvd = packet.sequenceNum;
	std::int32_t bufferOffset = static_cast<std::int32_t>(seqNumRecvd - m_lastSeqNumSeen - 1);
	if (bufferOffset > rpcGroups.size() - 1) {
		g_out << "WARNING: Missed packet count exceeds redundancy buffer size, may miss Remote Procedure Calls\n";
		bufferOffset = static_cast<std::int32_t>(rpcGroups.size()) - 1;
	}

	// Execute RPCs received from server
	for (std::int32_t i = bufferOffset; i >= 0; --i) {
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
				g_out << "Error: Ghost Snapshot with unassigned network id received by client\n";
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
					g_out << "Warning: Client received snapshot of deleted ghost\n";
				}
			} else {
				// TODO: Add logging here
				g_out << "Warning: Client received snapshot of non-existant ghost\n";
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
			m_scene.destroyEntity(*existingEntity);
			m_netEntities.at(entityNetId) = nullptr;
			return true;
		}
	}

	return false;
}
