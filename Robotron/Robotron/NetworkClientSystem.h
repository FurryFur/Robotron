#pragma once

#include "NetworkSystem.h"

#include <vector>
#include <memory>
#include <cstdint>

class Scene;
class Entity;
class PlayerInfo;
class Audio;
struct InputComponent;
struct Packet;
struct sockaddr_in;

enum ClientState {
	CLIENT_STATE_NO_SERVER,
	CLIENT_STATE_IN_LOBBY,
	CLIENT_STATE_IN_GAME
};

class NetworkClientSystem : public NetworkSystem
{
public:
	NetworkClientSystem(Scene&, Audio& audioSystem, const std::string& username);
	virtual ~NetworkClientSystem() override;

	virtual void beginFrame() override;
	virtual void update(Entity&, float deltaTick) override;
	virtual void endFrame() override;
	virtual bool isInGame() override;
	virtual void startGame() override;

	// Sends out a broadcast message for servers on the local network.
	// Changes the state of the client to CLIENT_STATE_NO_SERVER.
	void broadcastForServers();

	// Join the server at the specified address
	void joinServer(const sockaddr_in& address);

	// Update player information such as lives and score and new players.
	// This should be called via Remote Procedure Calls sent from the server.
	void updatePlayers(const std::vector<PlayerInfo>& currentPlayers);

	// Plays sound effects on the client.
	// This should be called via Remote Procedure Calls sent from the server.
	void playAudio(Sound);

	// This should be called via Remote Procedure Calls sent from the server.
	void createGhost(std::int32_t entityNetId, ModelID modelId);

	// This should be called via Remote Procedure Calls sent from the server.
	void createPlayerGhost(std::int32_t entityNetId, const PlayerInfo& playerInfo);

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
	std::uint8_t m_clientPlayerID;
	Audio& m_audioSystem;
	// The time at which the last packet was received
	std::chrono::time_point<std::chrono::high_resolution_clock> m_tLastPacketRecvd;
};

