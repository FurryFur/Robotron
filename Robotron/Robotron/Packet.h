#pragma once

#include "GhostSnapshot.h"
#include "StreamableEvictQueue.h"
#include "StreamableVector.h"
#include "RPC.h"

#include <cstdint>
#include <vector>

class OutBufferStream;

enum PacketType : std::uint8_t {
	PACKET_TYPE_NORMAL,
	PACKET_TYPE_BROADCAST,
	PACKET_TYPE_BROADCAST_RESPONSE,
	PACKET_TYPE_JOIN_REQUEST,
	PACKET_TYPE_JOIN_RESPONSE
};

struct Packet {
	Packet() {};
	Packet(const Packet&) = delete;
	Packet(Packet&&) = default;
	Packet& operator=(const Packet&) = delete;
	Packet& operator=(Packet&&) = default;

	PacketType packetType;

	// Broadcast response packet contents
	std::string serverName;

	// Join request packet contents
	std::string username;

	// Join response packet contents
	bool joinAccepted;

	// Normal packet contents
	std::uint32_t sequenceNum;
	StreamableVector<GhostSnapshot, 64> ghostSnapshotBuffer;
	StreamableEvictQueue<RPCGroup, 32> rpcGroupBuffer;

	OutBufferStream& serialize(OutBufferStream&) const;
	InBufferStream& deserialize(InBufferStream&);
};

OutBufferStream& operator<<(OutBufferStream&, const Packet&);
InBufferStream& operator>>(InBufferStream&, Packet&);