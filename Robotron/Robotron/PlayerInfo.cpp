#include "PlayerInfo.h"

#include "BufferStream.h"

OutBufferStream& operator<<(OutBufferStream& obs, const PlayerInfo& playerInfo)
{
	obs << playerInfo.username << playerInfo.lives << playerInfo.score;
	return obs;
}
