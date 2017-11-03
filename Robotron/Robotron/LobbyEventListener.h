#pragma once

#include "socket.h"

#include "ClientInfo.h"

class LobbyEventListener {
public:
	virtual void handleBroadcastResponse(const std::string& serverName, const sockaddr_in& serverAddress) = 0;
	virtual void handleJoinAccepted() = 0;
	virtual void handleJoinRejected() = 0;
	virtual void handleLobbyUpdate(const std::vector<PlayerInfo>&) = 0;
};