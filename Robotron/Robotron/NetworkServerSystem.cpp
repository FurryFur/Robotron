#include "NetworkServerSystem.h"

#include "Scene.h"
#include "Packet.h"
#include "Entity.h"
#include "Utils.h"

#include <algorithm>

NetworkServerSystem::NetworkServerSystem(Scene& scene)
	: NetworkSystem(scene)
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
	int& id = entity.network.id;
	bool isNewEntity = entity.network.isNewEntity;
	if (entity.componentMask == COMPONENT_NONE && id > -1) {
		// Notify clients of deletion
		// TODO: Add redundancy here, so that missed packets don't cause an
		// object to NOT be removed on the client side.
		Packet packet;
		packet.serialize(PACKET_TYPE_DESTROY, id);
		broadcastToClients(packet);

		// Remove entity from networking
		m_netEntities.at(id) = nullptr;
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
				entity.network.id = std::distance(m_netEntities.begin(), freeIt);
				*freeIt = &entity;
			} else {
				// Otherwise assign the new entity a brand new id
				entity.network.id = m_netEntities.size();
				m_netEntities.push_back(&entity);
			}
		}

		// Update state to reflect that this entity has been seen by the
		// network system.
		entity.network.isNewEntity = false;

		// Broadcast ghost entity creation command to clients
		Packet packet;
		packet.serialize(PACKET_TYPE_CREATE_GHOST, id, entity.transform);
		broadcastToClients(packet);
	}

	if (entity.hasAllComponents(COMPONENT_NETWORK | COMPONENT_TRANSFORM)) {
		// Update all clients with new entity transform snapshots
		Packet packet;
		packet.serialize(id, entity.transform);
		broadcastToClients(packet);
	}
}

void NetworkServerSystem::broadcastToClients(Packet packet)
{
	for (auto& address : m_clients) {
		sendData(packet, address);
	}
}
