#include "Packet.h"

void Packet::serialize(size_t entityNetID, glm::mat4 transform)
{
	this->type = PACKET_TYPE_TRANSFORM;
	this->entityNetID = entityNetID;
	this->transform = transform;
}

void Packet::serialize(size_t entityNetID, PhysicsComponent ghostSnapshot)
{
	this->type = PACKET_TYPE_GHOST_SNAPSHOT;
	this->entityNetID = entityNetID;
	this->ghostSnapshot = ghostSnapshot;
}

void Packet::serialize(size_t entityNetID, InputComponent input)
{
	this->type = PACKET_TYPE_INPUT;
	this->entityNetID = entityNetID;
	this->input = input;
}
