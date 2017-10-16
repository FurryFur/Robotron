#pragma once

#include "InputComponent.h"
#include "PhysicsComponent.h"

#include <glm\glm.hpp>

enum PacketType {
	PACKET_TYPE_GHOST_SNAPSHOT,
	PACKET_TYPE_TRANSFORM,
	PACKET_TYPE_INPUT,
};

struct Packet {
	PacketType type;

	union {
		PhysicsComponent ghostSnapshot;
		glm::mat4 transform;
		InputComponent input;
	};

	Packet() {}

	void serialize(glm::mat4 transform);
	void serialize(PhysicsComponent ghostSnapshot);
	void serialize(InputComponent input);
};