#include "NetworkClientSystem.h"

#include "Scene.h"
#include "Packet.h"
#include "EntityUtils.h"
#include "Entity.h"
#include "GhostSnapshot.h"

#include <glm\gtx\matrix_interpolation.hpp>
#include <glm\gtx\compatibility.hpp>

#include <cmath>
#include <iostream>
#include <cstdint>

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
		auto& ghostSnapshots = m_recvPacket.ghostSnapshotBuffer;
		auto& rpcGroups = m_recvPacket.rpcGroupBuffer;

		// Get figure out which remote procedure calls we got from the server
		// that we haven't seen before.
		std::uint32_t seqNumRecvd = m_recvPacket.sequenceNum;
		std::uint32_t bufferOffset = std::min((seqNumRecvd - m_lastSeqNumSeen - 1),
			(static_cast<std::uint32_t>(rpcGroups.size() - 1)));

		// Execute RPCs received from server
		for (int i = bufferOffset; i >= 0; --i) {
			RPCGroup& rpcGroup = rpcGroups.at(i);
			for (auto& rpc : rpcGroup.getRpcs()) {
				rpc->execute(m_netEntities);
			}
		}

		if (seqNumRecvd > m_lastSeqNumSeen) {
			// Save ghost snapshots when new data received
			for (std::uint16_t i = 0; i < ghostSnapshots.size(); ++i) {
				GhostSnapshot& ghostSnapshot = ghostSnapshots.at(i);
				std::int32_t id = ghostSnapshot.entityNetId;

				// TODO: Add logging here
				if (id < 0) {
					std::cout << "Error: Ghost Snapshot with unassigned network id received by client" << std::endl;
					return;
				}

				// Make sure we don't try to access an out of range element
				if (id >= m_ghostSnapshots.size())
					m_ghostSnapshots.resize(id + 1);

				// Record new snapshot
				m_ghostSnapshots.at(id) = std::make_unique<GhostSnapshot>(ghostSnapshot);
			}

			// Update the last sequence number seen from the server
			m_lastSeqNumSeen = seqNumRecvd;
		}

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

void NetworkClientSystem::update(Entity& entity, float deltaTick)
{
	if (!entity.hasComponents(COMPONENT_NETWORK, COMPONENT_TRANSFORM))
		return;

	std::int32_t& id = entity.network.id;

	// TODO: Add logging here
	if (id < 0) {
		std::cout << "Error: Entity with unassigned network id seen by client" << std::endl;
		return;
	}

	// Make sure we don't try to access an out of range element
	if (id >= m_netEntities.size())
		m_netEntities.resize(id + 1);

	// Detect new entities
	if (!m_netEntities.at(id))
		m_netEntities.at(id) = &entity;

	// Update ghost snapshotsaa
	if (id < m_ghostSnapshots.size() && m_ghostSnapshots.at(id)) {
		// TODO: Add timestamps to lerp
		glm::mat4& eTransform = entity.transform;
		glm::mat4& gTransform = m_ghostSnapshots.at(id)->transform;
		PhysicsComponent& ePhysics = entity.physics;
		PhysicsComponent& gPhysics = m_ghostSnapshots.at(id)->physics;
		float lerpAmount = deltaTick * 20; // Lerps all the way in 50 milliseconds
		if (entity.hasComponents(COMPONENT_TRANSFORM)) {
			// TODO: Add delta tick here
			// Lerp transform component to latest snapshot
			eTransform = glm::interpolate(eTransform, gTransform, lerpAmount);
		}
		if (entity.hasComponents(COMPONENT_PHYSICS)) {
			// Lerp physics component to latest snapshot
			ePhysics.velocity = glm::lerp(ePhysics.velocity, gPhysics.velocity, lerpAmount);
			ePhysics.acceleration = glm::lerp(ePhysics.acceleration, gPhysics.acceleration, lerpAmount);
		}
	}
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
