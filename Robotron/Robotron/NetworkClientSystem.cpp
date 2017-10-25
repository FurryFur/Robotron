#include "NetworkClientSystem.h"

#include "Scene.h"
#include "Packet.h"
#include "EntityUtils.h"
#include "Entity.h"

NetworkClientSystem::NetworkClientSystem(Scene& scene)
	: NetworkSystem(scene)
{
	m_socket.initialise(4567);
	allocateRecvBuffer();
}

void NetworkClientSystem::beginFrame()
{
	Packet packet;
	sockaddr_in address;
	while (receiveData(packet, address)) {
	//	if (packet.type == PACKET_TYPE_CREATE_GHOST) {
	//		// If a ghost entity with this id already exists, then destroy it first
	//		destroyIfExists(packet.entityNetID);

	//		// TODO: Create different models based on model enum
	//		Entity& entity = EntityUtils::createSphere(m_scene, packet.transform);

	//		// Add entity to list of networked entities
	//		if (packet.entityNetID >= m_netEntities.size())
	//			m_netEntities.resize(packet.entityNetID + 1);
	//		m_netEntities.at(packet.entityNetID) = &entity;
	//	}
	//	if (packet.type == PACKET_TYPE_TRANSFORM) {
	//		// TODO: Save snapshot somewhere to be interpolated to
	//		if (packet.entityNetID < m_netEntities.size() && m_netEntities.at(packet.entityNetID))
	//			m_netEntities.at(packet.entityNetID)->transform = packet.transform;
	//	}
	//	if (packet.type == PACKET_TYPE_DESTROY) {
	//		destroyIfExists(packet.entityNetID);
	//	}
	}
}

void NetworkClientSystem::update(Entity& entity)
{
	// Do interpolation with snapshots here
}

void NetworkClientSystem::endFrame()
{

}

void NetworkClientSystem::destroyIfExists(size_t entityNetID)
{
	if (entityNetID < m_netEntities.size()) {
		Entity* ghost = m_netEntities.at(entityNetID);
		if (ghost) {
			m_scene.destroyEntity(*ghost);
			m_netEntities.at(entityNetID) = nullptr;
		}
	}
}
