#include "NetworkServerSystem.h"

#include "Scene.h"
#include "Packet.h"

NetworkServerSystem::NetworkServerSystem(Scene& scene)
	: NetworkSystem(scene)
{
	m_socket.initialise(8456);

	// Temp socket for receiving from self for testing.
	//m_socket2.initialise(4567);
}

void NetworkServerSystem::beginFrame()
{
	Packet packet;
	while (receiveData(packet)) {
		if (packet.type == PACKET_TYPE_INPUT) {
			m_scene.entities.at(packet.entityNetID)->input = packet.input;
		}
	}
}

void NetworkServerSystem::update(Entity& entity)
{
	const size_t kNetworkedFilter = COMPONENT_TRANSFORM;
	if ((entity.componentMask == kNetworkedFilter)) {
		// TODO: Make serialize functions for the different packet types
		//Packet packet;
		//packet.serialize(entity.network.id, entity.transform);
		//
		//// Fill out address to send to (including port)
		//sockaddr_in address;
		//inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
		//address.sin_port = htons(4567);
		//address.sin_family = AF_INET;

		//// Send the snapshot
		//sendData(packet, address);
	}
}


