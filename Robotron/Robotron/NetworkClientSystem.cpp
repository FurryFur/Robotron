#include "NetworkClientSystem.h"

#include "Scene.h"
#include "Packet.h"

NetworkClientSystem::NetworkClientSystem(Scene& scene)
	: NetworkSystem(scene)
{
	m_socket.initialise(4567);
}

void NetworkClientSystem::beginFrame()
{
	Packet packet;
	while (receiveData(packet)) {
		if (packet.type == PACKET_TYPE_TRANSFORM) {
			// TODO: Save snapshot somewhere to be interpolated to
			m_scene.transformComponents.at(packet.entityNetID) = packet.transform;
		}
	}
}

void NetworkClientSystem::update(size_t entityID)
{
	// Do interpolation with snapshots here
}
