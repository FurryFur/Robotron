#include "NetworkClientSystem.h"

#include "Scene.h"
#include "Packet.h"
#include "EntityUtils.h"
#include "Entity.h"

#include <cmath>

NetworkClientSystem::NetworkClientSystem(Scene& scene)
	: NetworkSystem(scene)
	, m_lastSeqNumSeen{ 0 }
{
	m_socket.initialise(4567);
	allocateRecvBuffer();
}

void NetworkClientSystem::beginFrame()
{
	sockaddr_in address;
	while (receiveData(m_recvPacket, address)) {
		// Get figure out which remote procedure calls we got from the server
		// that we haven't seen before.
		std::uint32_t seqNumRecvd = m_recvPacket.sequenceNum;
		std::uint32_t bufferOffset = std::min((seqNumRecvd - m_lastSeqNumSeen - 1),
			(static_cast<std::uint32_t>(m_recvPacket.rpcGroupBuffer.size() - 1)));

		// Execute RPCs received from server
		for (int i = bufferOffset; i >= 0; --i) {
			RPCGroup& rpcGroup = m_recvPacket.rpcGroupBuffer.at(i);
			for (auto& rpc : rpcGroup.getRpcs()) {
				rpc->execute(m_netEntities);
			}
		}

		// Update the last sequence number seen from the server
		if (seqNumRecvd > m_lastSeqNumSeen)
			m_lastSeqNumSeen = seqNumRecvd;

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
