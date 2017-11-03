#pragma once

#include "PlayerInfo.h"
#include "Entity.h"
#include "EntityUtils.h"

#include <glm\glm.hpp>

#include <vector>
#include <memory>
#include <cstdint>

class OutBufferStream;
class InBufferStream;
class NetworkServerSystem;
class NetworkClientSystem;
struct InputComponent;

enum RPCType : std::uint8_t {
	RPC_CREATE_PLAYER_GHOST,
	RPC_CREATE_GHOST,
	RPC_DESTROY_GHOST,
	RPC_RECORD_INPUT
};

namespace RPC {
	void setClient(NetworkClientSystem*);
	void setServer(NetworkServerSystem*);
}

class RemoteProcedureCall {
public:
	RemoteProcedureCall() = default;
	RemoteProcedureCall(std::int32_t entityNetid);

	virtual void execute() = 0;
	virtual OutBufferStream& serialize(OutBufferStream&) const = 0;
	virtual InBufferStream& deserialize(InBufferStream&) = 0;

	std::int32_t getEntityNetId();

protected:
	std::int32_t m_entityNetId;
};

OutBufferStream& operator<<(OutBufferStream&, const RemoteProcedureCall&);
InBufferStream& operator>>(InBufferStream&, RemoteProcedureCall&);

class RPCCreatePlayerGhost : public RemoteProcedureCall {
public:
	RPCCreatePlayerGhost() = default;
	RPCCreatePlayerGhost(std::int32_t entityNetId, const PlayerInfo&,
		const TransformComponent& transform);

	virtual void execute() override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;
	virtual InBufferStream& deserialize(InBufferStream&) override;

private:
	PlayerInfo m_playerInfo;
	TransformComponent m_transform;
};

class RPCCreateGhost : public RemoteProcedureCall {
public:
	RPCCreateGhost() = default;
	RPCCreateGhost(std::int32_t entityNetId, ModelID modelId, 
		const TransformComponent& transform);

	virtual void execute() override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;
	virtual InBufferStream& deserialize(InBufferStream&) override;

private:
	ModelID m_modelId;
	TransformComponent m_transform;
};

class RPCDestroyGhost : public RemoteProcedureCall {
public:
	RPCDestroyGhost() = default;
	RPCDestroyGhost(std::int32_t entityNetId);

	virtual void execute() override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;
	virtual InBufferStream& deserialize(InBufferStream&) override;
};

// A remote procedure call sent from the client that updates input state
// on the server.
class RPCRecordInput : public RemoteProcedureCall {
public:
	RPCRecordInput() = default;
	RPCRecordInput(std::int32_t entityNetId, const InputComponent&);

	virtual void execute() override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;
	virtual InBufferStream& deserialize(InBufferStream&) override;
	
private:
	InputComponent m_input;
};

// Remote procedure call collection.
// These should be grouped by packet sequence number.
class RPCGroup {
public:
	void addRPC(std::unique_ptr<RemoteProcedureCall> rpc);
	OutBufferStream& serialize(OutBufferStream&) const;
	InBufferStream& deserialize(InBufferStream&);

	const std::vector<std::unique_ptr<RemoteProcedureCall>>& getRpcs() const;

private:
	std::vector<std::unique_ptr<RemoteProcedureCall>> m_rpcs;
};

OutBufferStream& operator<<(OutBufferStream&, const RPCGroup&);
InBufferStream& operator>>(InBufferStream&, RPCGroup&);