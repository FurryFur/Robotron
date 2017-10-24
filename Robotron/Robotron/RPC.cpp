#include "RPC.h"
#include "GLMUtils.h"

RPCGroup::RPCGroup(std::uint32_t sequenceNum)
	: m_sequenceNum{ sequenceNum }
{
}

void RPCGroup::addRPC(std::unique_ptr<RemoteProcedureCall> rpc)
{
	rpcs.push_back(std::move(rpc));
}

OutBufferStream & RPCGroup::serialize(OutBufferStream& obs) const
{
	// Sequence numbers will be stored implicitely by array index.
	// First write out the number of RPCs in the group
	obs << rpcs.size();
	for (auto& rpc : rpcs) {
		// Then write out each rpc in the group to the buffer.
		obs << *rpc;
	}

	return obs;
}

std::uint32_t RPCGroup::sequenceNum() const
{
	return m_sequenceNum;
}

OutBufferStream& operator<<(OutBufferStream& obs, ModelID modelID)
{
	obs << static_cast<std::uint8_t>(modelID);
	return obs;
}

OutBufferStream& operator<<(OutBufferStream& obs, RPCType rpcType)
{
	obs << static_cast<std::uint8_t>(rpcType);
	return obs;
}

OutBufferStream& operator<<(OutBufferStream& obs, const RemoteProcedureCall& rpc)
{
	return rpc.serialize(obs);
}

OutBufferStream& operator<<(OutBufferStream& obs, const RPCGroup& rpcGroup)
{
	return rpcGroup.serialize(obs);
}

RPCDestroyGhost::RPCDestroyGhost(std::int32_t entityNetId)
	: RemoteProcedureCall(entityNetId)
{
	
}

void RPCDestroyGhost::execute()
{
}

OutBufferStream & RPCDestroyGhost::serialize(OutBufferStream& obs) const
{
	obs << RPCType::RPC_DESTROY_GHOST << m_entityNetId;
	return obs;
}

RemoteProcedureCall::RemoteProcedureCall(std::int32_t entityNetid)
	: m_entityNetId{ entityNetid }
{
}

RPCCreatePlayerGhost::RPCCreatePlayerGhost(std::int32_t entityNetId, 
	const PlayerInfo& playerInfo, const glm::mat4 & transform)
	: RemoteProcedureCall(entityNetId)
	, m_playerInfo{ playerInfo }
	, m_transform{ transform }
{
}

void RPCCreatePlayerGhost::execute()
{
}

OutBufferStream& RPCCreatePlayerGhost::serialize(OutBufferStream& obs) const
{
	obs << RPC_CREATE_PLAYER_GHOST << m_entityNetId << m_playerInfo
	    << m_transform;
	return obs;
}

RPCCreateGhost::RPCCreateGhost(std::int32_t entityNetId, ModelID modelId, 
	glm::mat4 transform)
	: RemoteProcedureCall(entityNetId)
	, m_modelId{ modelId }
	, m_transform { transform }
{
}

void RPCCreateGhost::execute()
{
}

OutBufferStream& RPCCreateGhost::serialize(OutBufferStream& obs) const
{
	obs << RPCType::RPC_CREATE_GHOST << m_entityNetId << m_modelId
	    << m_transform;
	return obs;
}

RPCRecordInput::RPCRecordInput(std::int32_t entityNetId, const InputComponent& input)
	: RemoteProcedureCall(entityNetId)
	, m_input(input)
{
}

void RPCRecordInput::execute()
{
}

OutBufferStream & RPCRecordInput::serialize(OutBufferStream& obs) const
{
	obs << RPC_RECORD_INPUT << m_entityNetId << m_input;
	return obs;
}
