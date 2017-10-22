#pragma once

#include "InputComponent.h"
#include "PlayerInfo.h"

#include <glm\glm.hpp>

enum ModelID : std::uint8_t {
	MODEL_PLAYER_BULLET,
	MODEL_ENEMY_BULLET,
	MODEL_ENEMY_ZOMBIE,
	MODEL_ENEMY_SNAKE,
	MODEL_ENEMY_SHOOTER,
	MODEL_SCORE_PICKUP_1,
	MODEL_SCORE_PICKUP_2,
	MODEL_SCORE_PICKUP_3
};

struct RPCCreatePlayerGhost {
	int entityNetId;
	PlayerInfo playerInfo;
	bool isOtherPlayer;
	glm::mat4 transform;
};

struct RPCCreateGhost {
	int entityNetId;
	ModelID modelId;
	glm::mat4 transform;
};

struct RPCDestroyGhost {
	int entityNetId;
};

struct RPCRecordInput {
	int entityNetId;
	InputComponent input;
};