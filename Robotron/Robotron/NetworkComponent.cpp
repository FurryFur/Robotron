#include "NetworkComponent.h"

#include "PlayerInfo.h"

NetworkComponent::NetworkComponent()
	: id{ -1 }
	, priority{ 999 }
	, lastInputReceived{}
	, receivedInputThisFrame{ false }
{
}
