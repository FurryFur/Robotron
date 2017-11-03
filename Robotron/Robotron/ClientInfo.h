#pragma once

#include "PlayerInfo.h"

#include <cstdint>
#include <string>

class Entity;

struct ClientInfo {
	ClientInfo();

	std::uint32_t lastSeqNumSeen;
	PlayerInfo playerInfo;
	Entity* playerEntity;
};