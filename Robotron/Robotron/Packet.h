#pragma once

#include "InputComponent.h"
#include "PhysicsComponent.h"

enum PacketType {
	GhostSnapshot,
	Input
};

struct Packet {
	PacketType type;

	union {
		PhysicsComponent ghostSnapshot;
		InputComponent input;
	};

	Packet() {}
};