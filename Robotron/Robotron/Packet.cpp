#include "Packet.h"

void Packet::serialize(glm::mat4 transform)
{
	this->type = PACKET_TYPE_TRANSFORM;
	this->transform = transform;
}

void Packet::serialize(PhysicsComponent ghostSnapshot)
{
	this->type = PACKET_TYPE_GHOST_SNAPSHOT;
	this->ghostSnapshot = ghostSnapshot;
}

void Packet::serialize(InputComponent input)
{
	this->type = PACKET_TYPE_INPUT;
	this->input = input;
}
