#include "RPC.h"

#include "BufferStream.h"
#include "Scene.h"

#include <iostream>

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
		if (rpc)
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
	if (0 <= m_entityNetId && m_entityNetId < netEntities.size()) {
		Entity* netEntity = netEntities.at(m_entityNetId);
		if (netEntity) {
			netEntity->destroy();
		}
	}
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
	Scene* scene = Scene::getCurrentScene();
	if (scene) {
		// Destroy existing entities with the same id before creating new ones
		if (0 <= m_entityNetId && m_entityNetId < netEntities.size()) {
			Entity* existingEntity = netEntities.at(m_entityNetId);
			if (existingEntity)
				existingEntity->destroy();
		}

		Entity& newEntity = EntityUtils::createPlayerGhost(*scene, m_transform, m_entityNetId);
	}
	else {
		// TODO: Add logging here
		std::cout << "Warning: Could not execute RPC Create Ghost. Missing scene, "
			"did you forget to call Scene::makeSceneCurrent." << std::endl;
	}
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
	Scene* scene = Scene::getCurrentScene();
	if (scene) {
		// Destroy existing entities with the same id before creating new ones
		if (0 <= m_entityNetId && m_entityNetId < netEntities.size()) {
			Entity* existingEntity = netEntities.at(m_entityNetId);
			if (existingEntity)
				existingEntity->destroy();
		}

		Entity& newEntity = EntityUtils::createGhost(*scene, m_modelId, m_transform, m_entityNetId);
	} else {
		// TODO: Add logging here
		std::cout << "Warning: Could not execute RPC Create Ghost. Missing scene, "
			"did you forget to call Scene::makeSceneCurrent." << std::endl;
	}
}

OutBufferStream& RPCCreateGhost::serialize(OutBufferStream& obs) const
{
	obs << RPCType::RPC_CREATE_GHOST << m_entityNetId << m_modelId << m_transform;
	return obs;
}

InBufferStream& RPCCreateGhost::deserialize(InBufferStream& ibs)
{
	// TODO: Add error checking when deserializing model
	// Make sure we get a valid model id back
	return ibs >> m_entityNetId >> m_modelId >> m_transform;
}

RPCRecordInput::RPCRecordInput(std::int32_t entityNetId, const InputComponent& input)
	: RemoteProcedureCall(entityNetId)
	, m_input(input)
{
}

void RPCRecordInput::execute(std::vector<Entity*>& netEntities)
{
	if (m_entityNetId < 0) {
		// TODO: Do logging here
		std::cout << "Warning: Server received an RPC with an unassigned network ID" << std::endl;
	}

	if (m_entityNetId < netEntities.size()) {
		if (netEntities.at(m_entityNetId))
			netEntities.at(m_entityNetId)->input = m_input;
		else {
			// TODO: Add logging here
			std::cout << "Info: Received RPC for destroyed entity" << std::endl;
		}
	} else {
		// TODO: Add logging here
		std::cout << "Warning: Received RPC with out of range network id" << std::endl;
	}
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
