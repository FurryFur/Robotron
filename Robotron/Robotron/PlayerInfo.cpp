#include "PlayerInfo.h"

#include "BufferStream.h"

OutBufferStream& operator<<(OutBufferStream& obs, const PlayerInfo& playerInfo)
{
	return obs << playerInfo.username << playerInfo.lives << playerInfo.score;
}

InBufferStream& operator>>(InBufferStream& ibs, PlayerInfo& playerInfo)
{
	return ibs >> playerInfo.username >> playerInfo.lives >> playerInfo.score;
}
