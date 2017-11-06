#include "RPC.h"

#include "BufferStream.h"
#include "Scene.h"
#include "NetworkSystem.h"
#include "NetworkClientSystem.h"
#include "NetworkServerSystem.h"

#include "Log.h"

NetworkClientSystem* g_clientSystem = nullptr;
NetworkServerSystem* g_serverSystem = nullptr;

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
		case RPC_UPDATE_PLAYERS:
			rpc = std::make_unique<RPCUpdatePlayers>();
			break;
		case RPC_START_GAME:
			rpc = std::make_unique<RPCStartGame>();
			break;
		case RPC_PLAY_AUDIO:
			rpc = std::make_unique<RPCPlayAudio>();
			break;
		default:
			ibs.setError(IBS_ERROR_CORRUPT_DATA); // Error logging is done in setError
			break;
		}
		if (rpc)
			ibs >> *rpc;
	}
	return ibs;
}

const std::vector<std::unique_ptr<RemoteProcedureCall>>& RPCGroup::getRpcs() const
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
	: m_entityNetId(entityNetId)
{
}

void RPCDestroyGhost::execute()
{
	if (m_entityNetId < 0) {
		// TODO: Add logging here
		g_out << "ERROR: Received Destroy Ghost RPC with an unset network entity id\n";
		return;
	}

	if (g_clientSystem) {
		if (!g_clientSystem->destroyIfExistsInNetwork(m_entityNetId)) {
			// TODO: Add logging here
			g_out << "WARNING: Recieved a destroy ghost RPC for an entity which doesn't exist on the client. ID: " << m_entityNetId << "\n";
		}
	}
	else {
		// TODO: Add logging here
		g_out << "WARNING: Received destroy ghost RPC on a non-client, or RPC client not set\n";
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

RPCCreatePlayerGhost::RPCCreatePlayerGhost(std::int32_t entityNetId,
	const PlayerInfo& playerInfo)
	: m_entityNetId(entityNetId)
	, m_playerInfo(playerInfo)
{
}

void RPCCreatePlayerGhost::execute()
{
	if (m_entityNetId < 0) {
		// TODO: Add logging here
		g_out << "ERROR: Received Create Player Ghost RPC with an unset network entity id\n";
		return;
	}

	if (g_clientSystem) {
		g_clientSystem->createPlayerGhost(m_entityNetId, m_playerInfo);
	}
	else {
		// TODO: Add logging here
		g_out << "WARNING: Received create player ghost RPC on a non-client, or RPC client not set\n";
	}
}

OutBufferStream& RPCCreatePlayerGhost::serialize(OutBufferStream& obs) const
{
	return obs << RPC_CREATE_PLAYER_GHOST << m_entityNetId << m_playerInfo;
}

InBufferStream& RPCCreatePlayerGhost::deserialize(InBufferStream& ibs)
{
	return ibs >> m_entityNetId >> m_playerInfo;
}

RPCCreateGhost::RPCCreateGhost(std::int32_t entityNetId, ModelID modelId)
	: m_entityNetId(entityNetId)
	, m_modelId{ modelId }
{
}

void RPCCreateGhost::execute()
{
	if (m_entityNetId < 0) {
		// TODO: Add logging here
		g_out << "ERROR: Received a Create Ghost RPC with an unset network entity id\n";
		return;
	}

	if (g_clientSystem) {
		g_clientSystem->createGhost(m_entityNetId, m_modelId);
	} else {
		// TODO: Add logging here
		g_out << "WARNING: Received create ghost RPC on a non-client, or RPC client not set\n";
	}
}

OutBufferStream& RPCCreateGhost::serialize(OutBufferStream& obs) const
{
	obs << RPCType::RPC_CREATE_GHOST << m_entityNetId << m_modelId;
	return obs;
}

InBufferStream& RPCCreateGhost::deserialize(InBufferStream& ibs)
{
	// TODO: Add error checking when deserializing model
	// Make sure we get a valid model id back
	return ibs >> m_entityNetId >> m_modelId;
}

RPCRecordInput::RPCRecordInput(std::int32_t entityNetId, const InputComponent& input)
	: m_entityNetId(entityNetId)
	, m_input(input)
{
}

void RPCRecordInput::execute()
{
	if (m_entityNetId < 0) {
		// TODO: Add logging here
		g_out << "ERROR: Received Record Input RPC with an unset network entity id\n";
		return;
	}

	if (g_serverSystem)
		g_serverSystem->recordInput(m_entityNetId, m_input);
	else {
		// TODO: Add logging here
		g_out << "WARNING: Received input on a non-server, or RPC server not set\n";
	}

	//if (m_entityNetId < netEntities.size()) {
	//	if (netEntities.at(m_entityNetId))
	//		netEntities.at(m_entityNetId)->input = m_input;
	//	else {
	//		// TODO: Add logging here
	//		g_out << "Info: Received RPC for destroyed entity\n";
	//	}
	//} else {
	//	// TODO: Add logging here
	//	g_out << "Warning: Received RPC with out of range network id\n";
	//}
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

void RPC::setClient(NetworkClientSystem* clientSystem)
{
	g_clientSystem = clientSystem;
}

void RPC::setServer(NetworkServerSystem* serverSystem)
{
	g_serverSystem = serverSystem;
}

RPCUpdatePlayers::RPCUpdatePlayers(const std::vector<PlayerInfo>& playerInfoCollection)
	: m_currentPlayers{ playerInfoCollection }
{
}

void RPCUpdatePlayers::execute()
{
	if (g_clientSystem)
		g_clientSystem->updatePlayers(m_currentPlayers);
	else {
		// TODO: Add logging here
		g_out << "WARNING: Received lobby update RPC on a non-client, or RPC client not set\n";
	}
}

OutBufferStream& RPCUpdatePlayers::serialize(OutBufferStream& obs) const
{
	obs << RPC_UPDATE_PLAYERS << static_cast<std::uint8_t>(m_currentPlayers.size());
	for (const PlayerInfo& playerInfo : m_currentPlayers)
		obs << playerInfo;

	return obs;
}

InBufferStream& RPCUpdatePlayers::deserialize(InBufferStream& ibs)
{
	std::uint8_t size;
	ibs >> size;
	m_currentPlayers.resize(size);
	for (PlayerInfo& playerInfo : m_currentPlayers)
		ibs >> playerInfo;

	return ibs;
}

void RPCStartGame::execute()
{
	if (g_clientSystem)
		g_clientSystem->startGame();
	else {
		// TODO: Add logging here
		g_out << "WARNING: Received a Start Game RPC on a non-client, or RPC client not set\n";
	}
}

OutBufferStream& RPCStartGame::serialize(OutBufferStream& obs) const
{
	return obs << RPC_START_GAME;
}

InBufferStream& RPCStartGame::deserialize(InBufferStream& ibs)
{
	return ibs;
}

RPCPlayAudio::RPCPlayAudio(Sound sound)
	: m_sound{ sound }
{
}

void RPCPlayAudio::execute()
{
	if (g_clientSystem)
		g_clientSystem->playAudio(m_sound);
	else {
		// TODO: Add logging here
		g_out << "WARNING: Received a Play Audio RPC on a non-client, or RPC client not set\n";
	}
}

OutBufferStream& RPCPlayAudio::serialize(OutBufferStream& obs) const
{
	return obs << RPCType::RPC_PLAY_AUDIO << m_sound;
}

InBufferStream& RPCPlayAudio::deserialize(InBufferStream& ibs)
{
	// TODO: Add error checking for non-junk enum values
	return ibs >> m_sound;
}
