#include "PlayerInfo.h"

#include "BufferStream.h"

const std::uint8_t PlayerInfo::s_kDefaultLives = 2;

PlayerInfo::PlayerInfo()
	: m_lives{ s_kDefaultLives }
	, m_score{ 0 }
	, m_hasChanged{ false }
{
}

std::uint8_t PlayerInfo::getLives() const
{
	return m_lives;
}

std::uint32_t PlayerInfo::getScore() const
{
	return m_score;
}

void PlayerInfo::setLives(std::uint8_t lives)
{
	m_lives = lives;
	m_hasChanged = true;
}

void PlayerInfo::addLives(std::uint8_t livesToAdd)
{
	m_lives += livesToAdd;
	m_hasChanged = true;
}

void PlayerInfo::subtractLives(std::uint8_t livesToSubtract)
{
	m_lives -= livesToSubtract;
	m_hasChanged = true;
}

void PlayerInfo::setScore(std::uint32_t score)
{
	m_score = score;
	m_hasChanged = true;
}

void PlayerInfo::addScore(std::uint32_t scoreToAdd)
{
	m_score += scoreToAdd;
	m_hasChanged = true;
}

void PlayerInfo::subtractScore(std::uint32_t scoreToSubtract)
{
	m_score -= scoreToSubtract;
	m_hasChanged = true;
}

void PlayerInfo::setUniquePlayerID()
{
	static std::uint8_t s_freeID = 0;
	m_playerID = s_freeID;
	++s_freeID;
}

std::uint8_t PlayerInfo::getPlayerID() const
{
	return m_playerID;
}

bool PlayerInfo::hasChanged() const
{
	bool tmp = m_hasChanged;
	m_hasChanged = false;
	return tmp;
}

OutBufferStream & PlayerInfo::serialize(OutBufferStream& obs) const
{
	return obs << m_playerID << username << m_lives << m_score;
}

InBufferStream & PlayerInfo::deserialize(InBufferStream& ibs)
{
	return ibs >> m_playerID >> username >> m_lives >> m_score;
}

OutBufferStream& operator<<(OutBufferStream& obs, const PlayerInfo& playerInfo)
{
	return playerInfo.serialize(obs);
}

InBufferStream& operator >> (InBufferStream& ibs, PlayerInfo& playerInfo)
{
	return playerInfo.deserialize(ibs);
}