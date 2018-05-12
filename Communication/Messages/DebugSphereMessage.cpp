#include "DebugSphereMessage.h"


DebugSphereMessage::DebugSphereMessage(const std::string & destinationName, NCLVector3 position,
	float radius, NCLVector3 colour) : Message(destinationName, DEBUG_SPHERE)
{
	this->destination = destinationName;
	this->position = position;
	this->radius = radius;
	this->colour = colour;
}

DebugSphereMessage::~DebugSphereMessage()
{
}
