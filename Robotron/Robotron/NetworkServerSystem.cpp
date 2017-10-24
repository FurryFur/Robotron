#include "NetworkServerSystem.h"

#include "Scene.h"
#include "Packet.h"
#include "Entity.h"
#include "Utils.h"

#include <algorithm>

NetworkServerSystem::NetworkServerSystem(Scene& scene)
	: NetworkSystem(scene)
	, m_curSeqenceNum{ 0 }
{
	m_socket.initialise(8456);

	// TODO: Create client connection code instead of hard coding single
	// client
	// TEMP hard coded client (localhost:4567)
	sockaddr_in address;
	inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
	address.sin_port = htons(4567);
	address.sin_family = AF_INET;
	m_clients.insert(address);

	// Temp socket for receiving from self for testing.
	//m_socket2.initialise(4567);
}

void NetworkServerSystem::beginFrame()
{
	Packet packet;
	while (receiveData(packet)) {
		if (packet.type == PACKET_TYPE_INPUT) {
			m_netEntities.at(packet.entityNetID)->input = packet.input;
		}
	}
}

void NetworkServerSystem::update(Entity& entity)
{
	// Detect object deletion
	std::int32_t& id = entity.network.id;
	bool isNewEntity = entity.network.isNewEntity;
	if (entity.componentMask == COMPONENT_NONE && id > -1) {
		// Notify clients of deletion
		// TODO: Add redundancy here, so that missed packets don't cause an
		// object to NOT be removed on the client side.
		destroyNetworkEntity(id);
	}

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

	if (entity.hasAllComponents(COMPONENT_NETWORK | COMPONENT_TRANSFORM)) {
		// Update all clients with new entity transform snapshots
		Packet packet;
		packet.serialize(id, entity.transform);
		broadcastToClients(packet);
	}
}

void NetworkServerSystem::endFrame()
{
	OutBufferStream obs;
	obs << m_rpcGroupBuffer;
	obs.getData();
}

void NetworkServerSystem::destroyNetworkEntity(std::int32_t id)
{
	if (m_netEntities.at(id) == nullptr)
		return;

	if (m_netEntities.at(id)->componentMask != COMPONENT_NONE)
		m_netEntities.at(id)->componentMask = COMPONENT_NONE;

	auto rpc = std::make_unique<RPCDestroyGhost>(id);
	bufferRpc(std::move(rpc));

	// Stop tracking entity on the server
	m_netEntities.at(id) = nullptr;
}

void NetworkServerSystem::broadcastToClients(Packet packet)
{
	for (auto& address : m_clients) {
		sendData(packet, address);
	}
}

void NetworkServerSystem::bufferRpc(std::unique_ptr<RemoteProcedureCall> rpc)
{
	// Add a new rpc group to the buffer if the sequence number has increased
	RPCGroup* rpcGroup = m_rpcGroupBuffer.size() > 0 ? 
		&m_rpcGroupBuffer.front() : nullptr;
	if (!rpcGroup || rpcGroup->sequenceNum() < m_curSeqenceNum) {
		// Create a new RPCGroup for the current sequence number
		m_rpcGroupBuffer.emplace(m_curSeqenceNum);
		rpcGroup = &m_rpcGroupBuffer.front();
	}

	// Add the rpc to the latest RPCGroup
	rpcGroup->addRPC(std::move(rpc));
}
