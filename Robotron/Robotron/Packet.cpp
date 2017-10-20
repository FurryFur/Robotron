#include "Packet.h"

void Packet::serialize(int entityNetID, const glm::mat4& transform)
{
	this->type = PACKET_TYPE_TRANSFORM;
	this->entityNetID = entityNetID;
	this->transform = transform;
}

void Packet::serialize(int entityNetID, PhysicsComponent ghostSnapshot)
{
	this->type = PACKET_TYPE_GHOST_SNAPSHOT;
	this->entityNetID = entityNetID;
	this->ghostSnapshot = ghostSnapshot;
}

void Packet::serialize(int entityNetID, InputComponent input)
{
	this->type = PACKET_TYPE_INPUT;
	this->entityNetID = entityNetID;
	this->input = input;
}

void Packet::serialize(PacketType type, int entityNetID)
{
	this->type = type;
	this->entityNetID = entityNetID;
}

void Packet::serialize(PacketType type, int entityNetID, const glm::mat4& transform)
{
	this->type = type;
	this->entityNetID = entityNetID;
	this->transform = transform;
}