#include "NetworkServerSystem.h"

#include "Scene.h"
#include "Packet.h"
#include "Entity.h"
#include "Utils.h"
#include "ClientInfo.h"
#include "GhostSnapshot.h"

#include <algorithm>

using namespace std::chrono;

NetworkServerSystem::NetworkServerSystem(Scene& scene)
	: NetworkSystem(scene)
	, m_curSeqenceNum{ 0 }
	, m_packetInterval{ 50 }
	, m_willSendPcktThisFrame{ true }
{
	m_socket.initialise(8456);

	// TODO: Create client connection code instead of hard coding single
	// client
	// TEMP hard coded client (localhost:4567)
	sockaddr_in address;
	inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
	address.sin_port = htons(4567);
	address.sin_family = AF_INET;
	ClientInfo clientInfo;
	clientInfo.lastSeqNumSeen = 0;
	clientInfo.playerInfo.username = "Defaut";
	clientInfo.playerInfo.lives = 3;
	clientInfo.playerInfo.score = 0;
	m_clients.insert(std::make_pair(address, clientInfo));

	m_sendPacket.rpcGroupBuffer.emplace();

	// Temp socket for receiving from self for testing.
	//m_socket2.initialise(4567);
}

void NetworkServerSystem::receiveAndProcess()
{
	sockaddr_in address;
	while (receiveData(m_recvPacket, address)) {
		// Check we have a client associated with this address
		auto clientIt = m_clients.find(address);
		if (clientIt == m_clients.end())
			continue;

		// Get figure out which remote procedure calls we got from the client
		// that we haven't seen before.
		std::uint32_t seqNumSeen = clientIt->second.lastSeqNumSeen;
		std::uint32_t seqNumRecvd = m_recvPacket.sequenceNum;
		size_t bufferOffset = std::max(static_cast<size_t>(seqNumRecvd - seqNumSeen), 
			                            m_recvPacket.rpcGroupBuffer.size() - 1);
		
		// Execute RPCs received from client
		for (size_t i = bufferOffset; i >= 0; --i) {
			RPCGroup& rpcGroup = m_recvPacket.rpcGroupBuffer.at(i);
			for (auto& rpc : rpcGroup.getRpcs()) {
				// TODO: Add check to make sure rpc is being executed
				// on an entity that the client actually controls (security)
				rpc->execute(m_netEntities);
			}
		}

		// Update the last sequence number seen from the client
		clientIt->second.lastSeqNumSeen = seqNumRecvd;
	}
}

void NetworkServerSystem::beginFrame()
{
	// Receive and process packets
	receiveAndProcess();

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

void NetworkServerSystem::update(Entity& entity)
{
	std::int32_t& id = entity.network.id;
	bool isNewEntity = entity.network.isNewEntity;

	// Detect and handle entity destruction
	handleEntityDestruction(entity);

	// Detect new entities
	if (entity.hasAllComponents(COMPONENT_NETWORK) && isNewEntity) {
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
			} else {
				// Otherwise assign the new entity a brand new id
				entity.network.id = static_cast<std::int32_t>(m_netEntities.size());
				m_netEntities.push_back(&entity);
			}
		}

		// Update state to reflect that this entity has been seen by the
		// network system.
		entity.network.isNewEntity = false;

		// Broadcast ghost entity creation command to clients
		//if (entity.hasAllComponents(COMPONENT_PLAYER_CONTROL)) {
		//	//RPCCreatePlayerGhost(id, playerInfo, )
		//}

		std::unique_ptr<RPCCreateGhost> rpc;
		if (entity.hasAllComponents(COMPONENT_ENEMY01)) {
			rpc = std::make_unique<RPCCreateGhost>(id,
				ModelID::MODEL_ENEMY_ZOMBIE, entity.transform);
		} else if (entity.hasAllComponents(COMPONENT_ENEMY02)) {
			rpc = std::make_unique<RPCCreateGhost>(id,
				ModelID::MODEL_ENEMY_SHOOTER, entity.transform);
		} else if (entity.hasAllComponents(COMPONENT_ENEMY03)) {
			rpc = std::make_unique<RPCCreateGhost>(id,
				ModelID::MODEL_ENEMY_SNAKE, entity.transform);
		} else if (entity.hasAllComponents(COMPONENT_PLAYERBULLET)) {
			//  TODO: Add enemy bullets
			rpc = std::make_unique <RPCCreateGhost>(id,
				ModelID::MODEL_PLAYER_BULLET, entity.transform);
		} else if (entity.hasAllComponents(COMPONENT_SCOREPICKUP)) {
			// TODO: Add different pickup types
			rpc = std::make_unique <RPCCreateGhost>(id,
				ModelID::MODEL_SCORE_PICKUP_1, entity.transform);
		} else {
			rpc = nullptr;
		}

		if (rpc) {
			bufferRpc(std::move(rpc));
		}
	}

	if (m_willSendPcktThisFrame
	 && entity.hasAllComponents(COMPONENT_NETWORK | COMPONENT_TRANSFORM 
	                          | COMPONENT_PHYSICS)) {
		++entity.network.priority;
	}
}

void NetworkServerSystem::handleEntityDestruction(Entity& entity)
{
	std::int32_t& id = entity.network.id;
	if (entity.componentMask == COMPONENT_NONE && id > -1) {
		if (m_netEntities.at(id) == nullptr)
			return;

		if (m_netEntities.at(id)->componentMask != COMPONENT_NONE)
			m_netEntities.at(id)->componentMask = COMPONENT_NONE;

		auto rpc = std::make_unique<RPCDestroyGhost>(id);
		bufferRpc(std::move(rpc));

		// Stop tracking entity on the server
		m_netEntities.at(id) = nullptr;
	}
}

void NetworkServerSystem::endFrame()
{
	if (m_willSendPcktThisFrame) {
		m_sendPacket.sequenceNum = m_curSeqenceNum;
		selectGhostSnapshots(m_sendPacket.ghostSnapshotBuffer, 
		                     m_snapshotPriorityQ, 
		                     m_sendPacket.ghostSnapshotBuffer.getMaxSize());

		broadcastToClients(m_sendPacket);

		m_snapshotPriorityQ = {};
		++m_curSeqenceNum;
		// New RPC group for new sequence number
		m_sendPacket.rpcGroupBuffer.emplace();
	}
}

void NetworkServerSystem::broadcastToClients(const Packet& packet)
{
	for (auto& addressClientInfoPair : m_clients) {
		sendData(packet, addressClientInfoPair.first);
	}
}

void NetworkServerSystem::bufferRpc(std::unique_ptr<RemoteProcedureCall> rpc)
{
	// Add the rpc to the latest RPCGroup
	RPCGroup& rpcGroup = m_sendPacket.rpcGroupBuffer.front();
	rpcGroup.addRPC(std::move(rpc));
}

void NetworkServerSystem::selectGhostSnapshots(SnapshotBufT& dst, 
                                               PriorityQT& src,
                                               size_t maxSnapshots)
{
	dst.clear();
	for (size_t i = 0; i < maxSnapshots && !src.empty(); ++i) {
		Entity* entity = src.top();
		dst.emplace_back(entity->transform, entity->physics);
		entity->network.priority = 0;
	}
}
