#pragma once

#include "InputComponent.h"
#include "PhysicsComponent.h"

#include <glm\glm.hpp>

enum PacketType {
	PACKET_TYPE_CREATE_GHOST,
	PACKET_TYPE_GHOST_SNAPSHOT,
	PACKET_TYPE_TRANSFORM,
	PACKET_TYPE_INPUT,
	PACKET_TYPE_DESTROY
};

struct Packet {
	PacketType type;
	int entityNetID;

	union {
		PhysicsComponent ghostSnapshot;
		glm::mat4 transform;
		InputComponent input;
	};

	Packet() {}

	void serialize(int entityNetID, const glm::mat4& transform);
	void serialize(int entityNetID, PhysicsComponent ghostSnapshot);
	void serialize(int entityNetID, InputComponent input);
	void serialize(PacketType, int entityNetID);
	void serialize(PacketType, int entityNetID, const glm::mat4& transform);
};