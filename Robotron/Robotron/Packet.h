#pragma once

#include "GhostSnapshot.h"
#include "StreamableEvictQueue.h"
#include "StreamableVector.h"
#include "RPC.h"

#include <cstdint>
#include <vector>

class OutBufferStream;

struct Packet {
	Packet() = default;
	Packet(const Packet&) = delete;
	Packet(Packet&&) = default;
	Packet& operator=(const Packet&) = delete;
	Packet& operator=(Packet&&) = default;

	std::uint32_t sequenceNum;
	StreamableVector<GhostSnapshot, 64> ghostSnapshotBuffer;
	StreamableEvictQueue<RPCGroup, 32> rpcGroupBuffer;

	OutBufferStream& serialize(OutBufferStream&) const;
	InBufferStream& deserialize(InBufferStream&);
};

OutBufferStream& operator<<(OutBufferStream&, const Packet&);
InBufferStream& operator>>(InBufferStream&, Packet&);