#pragma once

#include <vector>
#include <string>

class PlayerInfo;
struct sockaddr_in;

class NetworkEventListener {
public:
	// Triggered when a client receives a response from a server to a broadcastForServers packet
	virtual void onBroadcastResponse(const std::string& serverName, const sockaddr_in& serverAddress) = 0;
	// Triggered when a client receives a join accept message from a server
	virtual void onJoinAccepted() = 0;
	// Triggered when a client receives a join rejection message from a server
	virtual void onJoinRejected() = 0;
	// Triggered when player information changes (lives, score, new players, disconnected players)
	virtual void onPlayersUpdated(const std::vector<PlayerInfo>&) = 0;
	// Triggered when the game is started on a client due to a message from the server
	virtual void onGameStart() = 0;
	// Triggered when a client detects that it has diconnected from the server.
	// Does not fire when this instance is the server and clients disconnect from it,
	// in this situation a Players Update event will be triggered instead.
	virtual void onDisconnect() = 0;
};