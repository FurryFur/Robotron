#pragma once

#include "PlayerInfo.h"
#include "Entity.h"
#include "EntityUtils.h"
#include "PlayerInfo.h"
#include "Audio.h"

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
	RPC_RECORD_INPUT,
	RPC_UPDATE_PLAYERS,
	RPC_START_GAME,
	RPC_PLAY_AUDIO
};

namespace RPC {
	void setClient(NetworkClientSystem*);
	void setServer(NetworkServerSystem*);
}

class RemoteProcedureCall {
public:
	RemoteProcedureCall() = default;
	virtual ~RemoteProcedureCall() {};

	virtual void execute() = 0;
	virtual OutBufferStream& serialize(OutBufferStream&) const = 0;
	virtual InBufferStream& deserialize(InBufferStream&) = 0;
};

OutBufferStream& operator<<(OutBufferStream&, const RemoteProcedureCall&);
InBufferStream& operator>>(InBufferStream&, RemoteProcedureCall&);

class RPCCreatePlayerGhost : public RemoteProcedureCall {
public:
	RPCCreatePlayerGhost() = default;
	RPCCreatePlayerGhost(std::int32_t entityNetId, const PlayerInfo&);

	virtual void execute() override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;
	virtual InBufferStream& deserialize(InBufferStream&) override;

private:
	std::int32_t m_entityNetId;
	PlayerInfo m_playerInfo;
};

class RPCCreateGhost : public RemoteProcedureCall {
public:
	RPCCreateGhost() = default;
	RPCCreateGhost(std::int32_t entityNetId, ModelID modelId);

	virtual void execute() override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;
	virtual InBufferStream& deserialize(InBufferStream&) override;

private:
	std::int32_t m_entityNetId;
	ModelID m_modelId;
};

class RPCDestroyGhost : public RemoteProcedureCall {
public:
	RPCDestroyGhost() = default;
	RPCDestroyGhost(std::int32_t entityNetId);

	virtual void execute() override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;
	virtual InBufferStream& deserialize(InBufferStream&) override;

private:
	std::int32_t m_entityNetId;
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
	std::int32_t m_entityNetId;
	InputComponent m_input;
};

// A remote procedure call sent from the server to update the lobby
// state of clients
class RPCUpdatePlayers : public RemoteProcedureCall {
public:
	RPCUpdatePlayers() = default;
	RPCUpdatePlayers(const std::vector<PlayerInfo>& playerInfoCollection);

	virtual void execute() override;
	virtual OutBufferStream& serialize(OutBufferStream &) const override;
	virtual InBufferStream& deserialize(InBufferStream &) override;

private:
	std::vector<PlayerInfo> m_currentPlayers;
};

// A remote procedure call sent from the server to start the game
// on the client machines
class RPCStartGame : public RemoteProcedureCall {
public:
	virtual void execute() override;
	virtual OutBufferStream& serialize(OutBufferStream&) const override;
	virtual InBufferStream& deserialize(InBufferStream&) override;
};

// A remote procedure call sent from the server to play audio sound 
// effects client game machines
class RPCPlayAudio : public RemoteProcedureCall {
public:
	RPCPlayAudio() = default;
	RPCPlayAudio(Sound);

	virtual void execute() override;
	virtual OutBufferStream & serialize(OutBufferStream &) const override;
	virtual InBufferStream & deserialize(InBufferStream &) override;

private:
	Sound m_sound;
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