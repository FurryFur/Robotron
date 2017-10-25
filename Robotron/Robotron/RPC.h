#pragma once

#include "InputComponent.h"
#include "PlayerInfo.h"
#include "BufferStream.h"
#include "Entity.h"

#include <glm\glm.hpp>

#include <vector>
#include <memory>
#include <cstdint>

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

	virtual void execute(std::vector<Entity*>& netEntities) = 0;
	virtual OutBufferStream& serialize(OutBufferStream&) const = 0;

	std::int32_t getEntityNetId();

protected:
	std::int32_t m_entityNetId;
};

OutBufferStream& operator<<(OutBufferStream&, const RemoteProcedureCall&);

class RPCCreatePlayerGhost : public RemoteProcedureCall {
	RPCCreatePlayerGhost(std::int32_t entityNetId, const PlayerInfo&,
		const glm::mat4& transform);

	virtual void execute(std::vector<Entity*>& netEntities) override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;

private:
	PlayerInfo m_playerInfo;
	glm::mat4 m_transform;
};

class RPCCreateGhost : public RemoteProcedureCall {
public:
	RPCCreateGhost(std::int32_t entityNetId, ModelID modelId, 
		glm::mat4 transform);

	virtual void execute(std::vector<Entity*>& netEntities) override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;

private:
	ModelID m_modelId;
	glm::mat4 m_transform;
};

class RPCDestroyGhost : public RemoteProcedureCall {
public:
	RPCDestroyGhost(std::int32_t entityNetId);

	virtual void execute(std::vector<Entity*>& netEntities) override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;
};

// A remote procedure call sent from the client that updates input state
// on the server.
class RPCRecordInput : public RemoteProcedureCall {
public:
	RPCRecordInput(std::int32_t entityNetId, const InputComponent&);

	virtual void execute(std::vector<Entity*>& netEntities) override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;
	
private:
	InputComponent m_input;
};

// Remote procedure call collection.
// These should be grouped by packet sequence number.
class RPCGroup {
public:
	void addRPC(std::unique_ptr<RemoteProcedureCall> rpc);
	OutBufferStream& serialize(OutBufferStream&) const;

	const std::vector<std::unique_ptr<RemoteProcedureCall>>& getRpcs();

private:
	std::vector<std::unique_ptr<RemoteProcedureCall>> m_rpcs;
};

OutBufferStream& operator<<(OutBufferStream&, const RPCGroup&);