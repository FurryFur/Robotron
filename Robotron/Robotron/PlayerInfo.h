#pragma once

#include <string>

class OutBufferStream;
class InBufferStream;

struct PlayerInfo {
	std::string username;
	std::uint8_t lives;
	std::uint32_t score;
};

OutBufferStream& operator<<(OutBufferStream&, const PlayerInfo&);
InBufferStream& operator>>(InBufferStream&, PlayerInfo&);