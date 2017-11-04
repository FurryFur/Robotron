#include "Packet.h"

#include "BufferStream.h"
#include "StreamableVector.h"
#include "StreamableEvictQueue.h"

OutBufferStream& Packet::serialize(OutBufferStream& obs) const
{
	obs << packetType;

	switch (packetType)
	{
	case PACKET_TYPE_BROADCAST:
		break;
	case PACKET_TYPE_BROADCAST_RESPONSE:
		obs << serverName;
		break;
	case PACKET_TYPE_JOIN_REQUEST:
		obs << username;
		break;
	case PACKET_TYPE_JOIN_RESPONSE:
		obs << joinAccepted << playerID;
		break;
	case PACKET_TYPE_NORMAL:
		obs << sequenceNum << ghostSnapshotBuffer << rpcGroupBuffer;
		break;
	default:
		break;
	}

	return obs;
}

InBufferStream& Packet::deserialize(InBufferStream& ibs)
{
	ibs >> packetType;

	switch (packetType)
	{
	case PACKET_TYPE_BROADCAST:
		break;
	case PACKET_TYPE_BROADCAST_RESPONSE:
		ibs >> serverName;
		break;
	case PACKET_TYPE_JOIN_REQUEST:
		ibs >> username;
		break;
	case PACKET_TYPE_JOIN_RESPONSE:
		ibs >> joinAccepted >> playerID;
		break;
	case PACKET_TYPE_NORMAL:
		ibs >> sequenceNum >> ghostSnapshotBuffer >> rpcGroupBuffer;
		break;
	default:
		break;
	}

	return ibs;
}

OutBufferStream& operator<<(OutBufferStream& obs, const Packet& packet)
{
	return packet.serialize(obs);
}

InBufferStream& operator>>(InBufferStream& ibs, Packet& packet)
{
	return packet.deserialize(ibs);
}
