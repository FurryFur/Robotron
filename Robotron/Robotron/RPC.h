#pragma once

#include "InputComponent.h"
#include "PlayerInfo.h"
#include "BufferStream.h"

#include <glm\glm.hpp>

#include <vector>
#include <memory>

struct InputComponent;

enum ModelID : std::uint8_t {
	MODEL_PLAYER_BULLET,
	MODEL_ENEMY_BULLET,
	MODEL_ENEMY_ZOMBIE,
	MODEL_ENEMY_SNAKE,
	MODEL_ENEMY_SHOOTER,
	MODEL_SCORE_PICKUP_1,
	MODEL_SCORE_PICKUP_2,
	MODEL_SCORE_PICKUP_3
};

enum RPCType : std::uint8_t {
	RPC_CREATE_PLAYER_GHOST,
	RPC_CREATE_GHOST,
	RPC_DESTROY_GHOST,
	RPC_RECORD_INPUT
};

OutBufferStream& operator<<(OutBufferStream&, ModelID);
OutBufferStream& operator<<(OutBufferStream&, RPCType);

class RemoteProcedureCall {
public:
	RemoteProcedureCall(std::int32_t entityNetid);

	virtual void execute() = 0;
	virtual OutBufferStream& serialize(OutBufferStream&) const = 0;

	friend OutBufferStream& operator<<(OutBufferStream&,
		const RemoteProcedureCall&);

protected:
	std::int32_t m_entityNetId;
};

class RPCCreatePlayerGhost : public RemoteProcedureCall {
	RPCCreatePlayerGhost(std::int32_t entityNetId, const PlayerInfo&,
		const glm::mat4& transform);

	virtual void execute() override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;

private:
	PlayerInfo m_playerInfo;
	glm::mat4 m_transform;
};

class RPCCreateGhost : public RemoteProcedureCall {
public:
	RPCCreateGhost(std::int32_t entityNetId, ModelID modelId, 
		glm::mat4 transform);

	virtual void execute() override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;

private:
	ModelID m_modelId;
	glm::mat4 m_transform;
};

class RPCDestroyGhost : public RemoteProcedureCall {
public:
	RPCDestroyGhost(std::int32_t entityNetId);

	virtual void execute() override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;
};

class RPCRecordInput : public RemoteProcedureCall {
public:
	RPCRecordInput(std::int32_t entityNetId, const InputComponent&);

	virtual void execute() override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;
	
private:
	InputComponent m_input;
};

// Remote procedure call collection.
// These should be grouped by packet sequence number.
class RPCGroup {
public:
	RPCGroup(std::uint32_t sequenceNum);

	void addRPC(std::unique_ptr<RemoteProcedureCall> rpc);
	OutBufferStream& serialize(OutBufferStream&) const;

	friend OutBufferStream& operator<<(OutBufferStream&, const RPCGroup&);

	std::uint32_t sequenceNum() const;

private:
	std::uint32_t m_sequenceNum;
	std::vector<std::unique_ptr<RemoteProcedureCall>> rpcs;
};