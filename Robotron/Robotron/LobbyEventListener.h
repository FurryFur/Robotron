#pragma once

#include "socket.h"

#include "ClientInfo.h"

class LobbyEventListener {
public:
	// Triggered when a client receives a response from a server to a broadcastForServers packet
	virtual void handleBroadcastResponse(const std::string& serverName, const sockaddr_in& serverAddress) = 0;
	// Triggered when a client receives a join accept message from a server
	virtual void handleJoinAccepted() = 0;
	// Triggered when a client receives a join rejection message from a server
	virtual void handleJoinRejected() = 0;
	// Triggered when player information (lives, score, new players) changes
	virtual void handleLobbyUpdate(const std::vector<PlayerInfo>&) = 0;
	// Triggered when the game is started on a client due to a message from the server
	virtual void handleGameStart() = 0;
	
	virtual void handleDisconnect() = 0;
};