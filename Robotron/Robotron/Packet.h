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
	size_t entityNetID;

	union {
		PhysicsComponent ghostSnapshot;
		glm::mat4 transform;
		InputComponent input;
	};

	Packet() {}

	void serialize(size_t entityNetID, const glm::mat4& transform);
	void serialize(size_t entityNetID, PhysicsComponent ghostSnapshot);
	void serialize(size_t entityNetID, InputComponent input);
	void serialize(PacketType, size_t entityNetID);
	void serialize(PacketType, size_t entityNetID, const glm::mat4& transform);
};