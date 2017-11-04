#pragma once

#include <string>

#include <cstdint>

class OutBufferStream;
class InBufferStream;

class PlayerInfo {
public:
	static const std::uint8_t s_kDefaultLives = 5;

	PlayerInfo();

	std::string username;
	std::uint8_t playerID; // The unique number the player is. ONLY CHANGED IN ENTITY AddComponent()

	std::uint8_t getLives() const;
	std::uint32_t getScore() const;
	void setLives(std::uint8_t);
	void addLives(std::uint8_t);
	void subtractLives(std::uint8_t);
	void setScore(std::uint32_t);
	void addScore(std::uint32_t);
	void subtractScore(std::uint32_t);

	// Checks if the lives or score of the player have
	// changed SINCE THIS FUNCTION WAS LAST CALLED.
	bool hasChanged() const;

	OutBufferStream& serialize(OutBufferStream&) const;
	InBufferStream& deserialize(InBufferStream&);

private:
	std::uint8_t m_lives;
	std::uint32_t m_score;
	
	mutable bool m_hasChanged;
};

OutBufferStream& operator<<(OutBufferStream&, const PlayerInfo&);
InBufferStream& operator>>(InBufferStream&, PlayerInfo&);