#pragma once

#include "PlayerInfo.h"

#include <cstdint>
#include <string>
#include <chrono>

class Entity;

struct ClientInfo {
	ClientInfo();

	std::uint32_t lastSeqNumSeen;
	std::chrono::time_point<std::chrono::high_resolution_clock> tLastPacketRecvd;
	PlayerInfo playerInfo;
	Entity* playerEntity;
};