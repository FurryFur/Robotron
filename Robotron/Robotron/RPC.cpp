#include "RPC.h"

#include "BufferStream.h"
#include "GLMUtils.h"

void RPCGroup::addRPC(std::unique_ptr<RemoteProcedureCall> rpc)
{
	m_rpcs.push_back(std::move(rpc));
}

OutBufferStream& RPCGroup::serialize(OutBufferStream& obs) const
{
	// Sequence numbers will be stored implicitely by array index.
	// First write out the number of RPCs in the group
	obs << static_cast<std::uint16_t>(m_rpcs.size());
	for (auto& rpc : m_rpcs) {
		// Then write out each rpc in the group to the buffer.
		obs << *rpc;
	}

	return obs;
}

InBufferStream& RPCGroup::deserialize(InBufferStream& ibs)
{
	if (ibs.getError())
		return ibs;

	std::uint16_t size;
	ibs >> size;
	m_rpcs.resize(size);
	std::unique_ptr<RemoteProcedureCall> rpc;
	for (auto& rpc : m_rpcs) {
		RPCType rpcType;
		ibs >> rpcType;
		switch (rpcType)
		{
		case RPC_CREATE_PLAYER_GHOST:
			rpc = std::make_unique<RPCCreatePlayerGhost>();
			break;
		case RPC_CREATE_GHOST:
			rpc = std::make_unique<RPCCreateGhost>();
			break;
		case RPC_DESTROY_GHOST:
			rpc = std::make_unique<RPCDestroyGhost>();
			break;
		case RPC_RECORD_INPUT:
			rpc = std::make_unique<RPCRecordInput>();
			break;
		default:
			ibs.setError(IBS_ERROR_CORRUPT_DATA);
			break;
		}
		ibs >> *rpc;
	}
	return ibs;
}

const std::vector<std::unique_ptr<RemoteProcedureCall>>& RPCGroup::getRpcs()
{
	return m_rpcs;
}

OutBufferStream& operator<<(OutBufferStream& obs, const RemoteProcedureCall& rpc)
{
	return rpc.serialize(obs);
}

InBufferStream& operator>>(InBufferStream& ibs, RemoteProcedureCall& rpc)
{
	return rpc.deserialize(ibs);
}

OutBufferStream& operator<<(OutBufferStream& obs, const RPCGroup& rpcGroup)
{
	return rpcGroup.serialize(obs);
}

InBufferStream& operator>>(InBufferStream& ibs, RPCGroup& rpcGroup)
{
	return rpcGroup.deserialize(ibs);
}

RPCDestroyGhost::RPCDestroyGhost(std::int32_t entityNetId)
	: RemoteProcedureCall(entityNetId)
{
}

void RPCDestroyGhost::execute(std::vector<Entity*>& netEntities)
{
}

OutBufferStream& RPCDestroyGhost::serialize(OutBufferStream& obs) const
{
	obs << RPCType::RPC_DESTROY_GHOST << m_entityNetId;
	return obs;
}

InBufferStream& RPCDestroyGhost::deserialize(InBufferStream& ibs)
{
	return ibs >> m_entityNetId;
}

RemoteProcedureCall::RemoteProcedureCall(std::int32_t entityNetid)
	: m_entityNetId{ entityNetid }
{
}

std::int32_t RemoteProcedureCall::getEntityNetId()
{
	return m_entityNetId;
}

RPCCreatePlayerGhost::RPCCreatePlayerGhost(std::int32_t entityNetId,
	const PlayerInfo& playerInfo, const glm::mat4& transform)
	: RemoteProcedureCall(entityNetId)
	, m_playerInfo(playerInfo)
	, m_transform{ transform }
{
}

void RPCCreatePlayerGhost::execute(std::vector<Entity*>& netEntities)
{
}

OutBufferStream& RPCCreatePlayerGhost::serialize(OutBufferStream& obs) const
{
	obs << RPC_CREATE_PLAYER_GHOST << m_entityNetId << m_playerInfo
	    << m_transform;
	return obs;
}

InBufferStream& RPCCreatePlayerGhost::deserialize(InBufferStream& ibs)
{
	return ibs >> m_entityNetId >> m_playerInfo >> m_transform;
}

RPCCreateGhost::RPCCreateGhost(std::int32_t entityNetId, ModelID modelId, 
	const glm::mat4& transform)
	: RemoteProcedureCall(entityNetId)
	, m_modelId{ modelId }
	, m_transform { transform }
{
}

void RPCCreateGhost::execute(std::vector<Entity*>& netEntities)
{
}

OutBufferStream& RPCCreateGhost::serialize(OutBufferStream& obs) const
{
	obs << RPCType::RPC_CREATE_GHOST << m_entityNetId << m_modelId << m_transform;
	return obs;
}

InBufferStream& RPCCreateGhost::deserialize(InBufferStream& ibs)
{
	return ibs >> m_entityNetId >> m_modelId >> m_transform;
}

RPCRecordInput::RPCRecordInput(std::int32_t entityNetId, const InputComponent& input)
	: RemoteProcedureCall(entityNetId)
	, m_input(input)
{
}

void RPCRecordInput::execute(std::vector<Entity*>& netEntities)
{
}

OutBufferStream& RPCRecordInput::serialize(OutBufferStream& obs) const
{
	obs << RPC_RECORD_INPUT << m_entityNetId << m_input;
	return obs;
}

InBufferStream& RPCRecordInput::deserialize(InBufferStream& ibs)
{
	return ibs >> m_entityNetId >> m_input;
}
