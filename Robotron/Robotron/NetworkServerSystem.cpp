#include "NetworkServerSystem.h"

#include "Scene.h"
#include "Packet.h"
#include "Entity.h"
#include "Utils.h"
#include "ClientInfo.h"
#include "GhostSnapshot.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>

const USHORT NetworkServerSystem::s_kDefaultServerPort = 8456;

NetworkServerSystem::NetworkServerSystem(Scene& scene)
	: NetworkSystem(scene)
	, m_serverState{ SERVER_STATE_LOBBY_MODE }
{
	m_socket.initialise(s_kDefaultServerPort);
	allocateRecvBuffer();

	// TODO: Create client connection code instead of hard coding single
	// client
	// TEMP hard coded client (localhost:4567)
	//sockaddr_in address;
	//inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
	//address.sin_port = htons(4567);
	//address.sin_family = AF_INET;
	//ClientInfo clientInfo;
	//clientInfo.lastSeqNumSeen = 0;
	//clientInfo.playerInfo.username = "Default";
	//clientInfo.playerInfo.lives = 3;
	//clientInfo.playerInfo.score = 0;
	//m_clients.insert(std::make_pair(address, clientInfo));

	// Temp socket for receiving from self for testing.
	//m_socket2.initialise(4567);
}

void NetworkServerSystem::handleGamePackets(const Packet& packet, const sockaddr_in& address)
{
	// Check we have a client associated with this address
	auto clientIt = m_clients.find(address);
	if (clientIt == m_clients.end()) {
		// TODO: Add logging here
		std::cout << "INFO: Received a packet from an unassociated client" 
		          << std::endl;
		return;
	}

	// Get figure out which remote procedure calls we got from the client
	// that we haven't seen before.
	std::uint32_t seqNumSeen = clientIt->second.lastSeqNumSeen;
	std::uint32_t seqNumRecvd = packet.sequenceNum;
	std::uint32_t bufferOffset = std::min((seqNumRecvd - seqNumSeen - 1),
			                                (static_cast<std::uint32_t>(packet.rpcGroupBuffer.size() - 1)));

	// Ignore out of order RPCs
	if (seqNumRecvd > seqNumSeen + 1 && bufferOffset < packet.rpcGroupBuffer.size() - 1) {
		// TODO: Add logging here
		std::cout << "INFO: Server received and out of order packet" << std::endl;
		return;
	}
		
	// Execute RPCs received from client
	for (int i = bufferOffset; i >= 0; --i) {
		const RPCGroup& rpcGroup = packet.rpcGroupBuffer.at(i);
		for (auto& rpc : rpcGroup.getRpcs()) {
			// TODO: Add check to make sure rpc is being executed
			// on an entity that the client actually controls (security)
			rpc->execute(m_netEntities);
		}
	}

	// Update the last sequence number seen from the client
	if (seqNumRecvd > seqNumSeen)
		clientIt->second.lastSeqNumSeen = seqNumRecvd;
}

void NetworkServerSystem::handleLobbyPackets(const Packet& packet, const sockaddr_in& address)
{
	switch (packet.packetType)
	{
	case PACKET_TYPE_NORMAL:
		handleGamePackets(packet, address);
		break;
	case PACKET_TYPE_BROADCAST:
		handleBroadcastPacket(packet, address);
		break;
	case PACKET_TYPE_JOIN_REQUEST:
		handleJoinPacket(packet, address);
		break;
	default:
		break;
	}
}

void NetworkServerSystem::handleBroadcastPacket(const Packet& packet, const sockaddr_in& address)
{
	// TODO: Add logging here
	std::cout << "INFO: Server received a broadcast packet from address: " 
	          << toString(address) << std::endl;

	Packet broadcastResp;
	broadcastResp.packetType = PACKET_TYPE_BROADCAST_RESPONSE;
	// TODO: Add server name from UI input
	broadcastResp.serverName = "Server Name Goes Here";
	sendData(broadcastResp, address);
}

void NetworkServerSystem::handleJoinPacket(const Packet& packet, const sockaddr_in& address)
{
	// TODO: Add logging here
	std::cout << "INFO: Server received a join request from user: " 
	          << packet.username << ", at address: " << toString(address) 
	          << std::endl;

	ClientInfo client;
	m_clients.insert(std::make_pair(address, client));

	Packet joinResp;
	joinResp.packetType = PACKET_TYPE_JOIN_RESPONSE;
	joinResp.joinAccepted = true; // Just always accept join requests for now
	sendData(joinResp, address);

	// TODO: Don't auto start the game, stay in lobby until game is started
	startGame();
}

void NetworkServerSystem::addToNetworking(Entity& entity)
{
	entity.addComponents(COMPONENT_NETWORK);

	std::int32_t id = entity.network.id;

	// Check if we can reuse the entities current id
	if (0 <= id && id < m_netEntities.size())
		// Reuse existing id from old entity
		m_netEntities.at(id) = &entity;
	else {
		// Check for a free id
		auto freeIt = std::find(m_netEntities.begin(), m_netEntities.end(), nullptr);
		if (freeIt != m_netEntities.end()) {
			// Assign the new entity a free id if found
			entity.network.id = static_cast<std::int32_t>(std::distance(m_netEntities.begin(), freeIt));
			*freeIt = &entity;
		}
		else {
			// Otherwise assign the new entity a brand new id
			entity.network.id = static_cast<std::int32_t>(m_netEntities.size());
			m_netEntities.push_back(&entity);
		}
	}

	// Update state to reflect that this entity has been seen by the
	// network system.
	entity.network.isNewEntity = false;

	// Create remote procedure calls to inform clients of new entity creation
	id = entity.network.id;
	std::unique_ptr<RemoteProcedureCall> rpc;
	if (entity.hasComponents(COMPONENT_PLAYER)) {
		// TODO: Replace this
		PlayerInfo playerInfo;
		playerInfo.lives = 3;
		playerInfo.score = 0;
		playerInfo.username = "asdfsdf";
		rpc = std::make_unique<RPCCreatePlayerGhost>(id, playerInfo, entity.transform);
	}
	else if (entity.hasComponents(COMPONENT_ZOMBIE)) {
		rpc = std::make_unique<RPCCreateGhost>(id, ModelID::MODEL_ENEMY_ZOMBIE,
			entity.transform);
	}
	else if (entity.hasComponents(COMPONENT_SNAKE)) {
		rpc = std::make_unique<RPCCreateGhost>(id, ModelID::MODEL_ENEMY_SNAKE,
			entity.transform);
	}
	else if (entity.hasComponents(COMPONENT_ENEMY_SHOOTER)) {
		rpc = std::make_unique<RPCCreateGhost>(id, ModelID::MODEL_ENEMY_SHOOTER,
			entity.transform);
	}
	else if (entity.hasComponents(COMPONENT_PLAYERBULLET)) {
		rpc = std::make_unique <RPCCreateGhost>(id, ModelID::MODEL_PLAYER_BULLET,
			entity.transform);
	}
	else if (entity.hasComponents(COMPONENT_ENEMYBULLET)) {
		rpc = std::make_unique <RPCCreateGhost>(id, ModelID::MODEL_ENEMY_BULLET,
			entity.transform);
	}
	else if (entity.hasComponents(COMPONENT_SCOREPICKUP)) {
		// TODO: Add different pickup types
		rpc = std::make_unique <RPCCreateGhost>(id, ModelID::MODEL_SCORE_PICKUP_1,
			entity.transform);
	}
	else {
		rpc = nullptr;
	}

	if (rpc) {
		bufferRpc(std::move(rpc));
	}
}

void NetworkServerSystem::beginFrame()
{
	sockaddr_in address;
	while (receiveData(m_recvPacket, address)) {
		// Receive and process packets
		switch (m_serverState)
		{
		case SERVER_STATE_LOBBY_MODE:
			handleLobbyPackets(m_recvPacket, address);
			break;
		case SERVER_STATE_IN_GAME:
			handleGamePackets(m_recvPacket, address);
			break;
		default:
			break;
		}
	}
	
	NetworkSystem::beginFrame();
}

void NetworkServerSystem::update(Entity& entity, float deltaTick)
{
	if (m_serverState != SERVER_STATE_IN_GAME)
		return;

	std::int32_t id = entity.network.id;
	bool isNewEntity = entity.network.isNewEntity;

	// Detect and handle entity destruction
	handleEntityDestruction(entity);

	// Detect new entities
	if (entity.hasComponents(COMPONENT_NETWORK) && isNewEntity) {
		addToNetworking(entity);
	}
}

void NetworkServerSystem::handleEntityDestruction(Entity& entity)
{
	if (!entity.hasComponents() && entity.hadComponentsPrev(COMPONENT_NETWORK)) {
		std::int32_t id = entity.network.id;
		if (0 <= id && id < m_netEntities.size()) {
			if (!m_netEntities.at(id))
				return;

			auto rpc = std::make_unique<RPCDestroyGhost>(id);
			bufferRpc(std::move(rpc));

			// Stop tracking entity on the server
			m_netEntities.at(id) = nullptr;
		}
	}
}

void NetworkServerSystem::endFrame()
{
	if (m_willSendPcktThisFrame) {
		// Set packet sequence number
		m_sendPacket.sequenceNum = m_curSeqenceNum;

		selectGhostSnapshots(m_sendPacket.ghostSnapshotBuffer, m_netEntities,
		                     m_sendPacket.ghostSnapshotBuffer.getMaxSize());

		// Send packet to clients
		m_sendPacket.packetType = PACKET_TYPE_NORMAL;
		broadcastToClients(m_sendPacket);
	}

	NetworkSystem::endFrame();
}

bool NetworkServerSystem::isInGame()
{
	return m_serverState == SERVER_STATE_IN_GAME;
}

void NetworkServerSystem::startGame()
{
	m_serverState = SERVER_STATE_IN_GAME;

	// Create entities for each player to possess
	for (auto& addressClientInfoPair : m_clients) {
		addressClientInfoPair.first;

		Entity& newPlayer = EntityUtils::createPlayer(m_scene, glm::translate(glm::mat4{}, { 0, 1, 0 }));
		newPlayer.removeComponents(COMPONENT_INPUT_MAP); // Input will come from the clients
		addressClientInfoPair.second.playerEntity = &newPlayer;
		addToNetworking(newPlayer);
	}
}

void NetworkServerSystem::broadcastToClients(const Packet& packet)
{
	for (auto& addressClientInfoPair : m_clients) {
		sendData(packet, addressClientInfoPair.first);
	}
}

void NetworkServerSystem::selectGhostSnapshots(SnapshotBufT& dst, 
                                               std::vector<Entity*>& src,
                                               size_t maxSnapshots)
{
	using PriorityQT = std::priority_queue<Entity*, std::vector<Entity*>, EntityPriorityComparitor>;
	static PriorityQT s_snapshotPriorityQ;

	// Create priority queue of entities from src
	s_snapshotPriorityQ = {};
	for (Entity* entity : src) {
		if (entity) {
			++entity->network.priority;
			s_snapshotPriorityQ.push(entity);
		}
	}

	// Clear destination snapshot buffer
	dst.clear();

	// Pull snapshots from priority queue
	for (size_t i = 0; i < maxSnapshots && !s_snapshotPriorityQ.empty(); ++i) {
		Entity* entity = s_snapshotPriorityQ.top();
		s_snapshotPriorityQ.pop();
		dst.emplace_back(entity->network.id, entity->transform, entity->physics);
		entity->network.priority = 0;
	}
}
