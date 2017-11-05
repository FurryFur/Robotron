#pragma once

#include <string>

#include <cstdint>

class OutBufferStream;
class InBufferStream;

class PlayerInfo {
public:
	static const std::uint8_t s_kDefaultLives;

	PlayerInfo();

	std::string username;

	std::uint8_t getLives() const;
	std::uint32_t getScore() const;
	std::uint8_t getPlayerID() const;
	void setLives(std::uint8_t);
	void addLives(std::uint8_t);
	void subtractLives(std::uint8_t);
	void setScore(std::uint32_t);
	void addScore(std::uint32_t);
	void subtractScore(std::uint32_t);
	void setUniquePlayerID();

	// Checks if the lives or score of the player have
	// changed SINCE THIS FUNCTION WAS LAST CALLED.
	bool hasChanged() const;

	OutBufferStream& serialize(OutBufferStream&) const;
	InBufferStream& deserialize(InBufferStream&);

private:
	std::uint8_t m_lives;
	std::uint32_t m_score;
	std::uint8_t m_playerID;
	
	mutable bool m_hasChanged;
};

OutBufferStream& operator<<(OutBufferStream&, const PlayerInfo&);
InBufferStream& operator>>(InBufferStream&, PlayerInfo&);