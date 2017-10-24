#pragma once

#include <string>

class OutBufferStream;

struct PlayerInfo {
	std::string username;
	std::uint8_t lives;
	std::uint32_t score;
};

OutBufferStream& operator<<(OutBufferStream& obs, const PlayerInfo& playerInfo);