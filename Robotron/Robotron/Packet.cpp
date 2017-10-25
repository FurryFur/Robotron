#include "Packet.h"

#include "BufferStream.h"
#include "StreamableVector.h"
#include "StreamableEvictQueue.h"

OutBufferStream& Packet::serialize(OutBufferStream& obs) const
{
	return obs << sequenceNum << ghostSnapshotBuffer << rpcGroupBuffer;
}

InBufferStream& Packet::deserialize(InBufferStream& ibs)
{
	return ibs >> sequenceNum >> ghostSnapshotBuffer >> rpcGroupBuffer;
}

OutBufferStream& operator<<(OutBufferStream& obs, const Packet& packet)
{
	return packet.serialize(obs);
}

InBufferStream& operator>>(InBufferStream& ibs, Packet& packet)
{
	return packet.deserialize(ibs);
}
