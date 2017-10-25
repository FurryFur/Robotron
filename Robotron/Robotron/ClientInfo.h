#pragma once

#include "PlayerInfo.h"

#include <cstdint>

struct ClientInfo {
	std::uint32_t lastSeqNumSeen;
	PlayerInfo playerInfo;
};