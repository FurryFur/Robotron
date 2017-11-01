#pragma once

#include "PlayerInfo.h"

#include <cstdint>

class Entity;

struct ClientInfo {
	ClientInfo();

	std::uint32_t lastSeqNumSeen;
	Entity* playerEntity;
};