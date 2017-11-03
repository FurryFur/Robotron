#pragma once

#include "NetworkSystem.h"
#include "GhostSnapshot.h"
#include "PlayerInfo.h"
#include "RPC.h"

#include <glm\glm.hpp>

#include <vector>
#include <memory>
#include <cstdint>

class Scene;
class Entity;
struct InputComponent;
struct Packet;

enum ClientState {
	CLIENT_STATE_NO_SERVER,
	CLIENT_STATE_JOINING,
	CLIENT_STATE_IN_LOBBY,
	CLIENT_STATE_IN_GAME
};

class NetworkClientSystem : public NetworkSystem
{
public:
	NetworkClientSystem(Scene&, const std::string& username);

	virtual void beginFrame() override;
	virtual void update(Entity&, float deltaTick) override;
	virtual void endFrame() override;
	virtual bool isInGame() override;

	// Sends out a broadcast message for servers on the local network.
	// Changes the state of the client to CLIENT_STATE_NO_SERVER.
	void broadcastForServers();

	// Join the server at the specified address
	void joinServer(const sockaddr_in& address);

	void updatePlayers(const std::vector<PlayerInfo>& currentPlayers);

	void createGhost(std::int32_t entityNetId, ModelID modelId, const TransformComponent& transform);
	void createPlayerGhost(std::int32_t entityNetId, const PlayerInfo& playerInfo, const TransformComponent& transform);

	// Destroys the entity if one exists at the specified
	// network id.
	// Returns true if an entity was destroyed
	bool destroyIfExistsInNetwork(std::int32_t entityNetId);
private:
	// Check for and handle broadcast responses and join responses from server
	void handlePreLobbyPackets(const Packet&, const sockaddr_in& address);

	// Handle game state updates received from the server
	void handleGamePackets(const Packet&, const sockaddr_in& address);

	std::uint32_t m_lastSeqNumSeen;
	//std::vector<std::unique_ptr<GhostSnapshot>> m_ghostSnapshots;
	sockaddr_in m_serverAddress;
	ClientState m_clientState;
	std::string m_username;
};

