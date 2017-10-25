#include "Packet.h"

#include "BufferStream.h"
#include "StreamableVector.h"
#include "StreamableEvictQueue.h"

OutBufferStream& Packet::serialize(OutBufferStream& obs) const
{
	return obs << sequenceNum << ghostSnapshotBuffer << rpcGroupBuffer;
}

OutBufferStream& operator<<(OutBufferStream& obs, const Packet& packet)
{
	return packet.serialize(obs);
}
