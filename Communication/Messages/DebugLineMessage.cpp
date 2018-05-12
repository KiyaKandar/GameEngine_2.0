#include "DebugLineMessage.h"



DebugLineMessage::DebugLineMessage(const std::string &destinationName, NCLVector3 from,
	NCLVector3 to, NCLVector3 colour) : Message(destinationName, DEBUG_LINE)
{
	this->destination = destinationName;
	this->from = from;
	this->to = to;
	this->colour = colour;
}


DebugLineMessage::~DebugLineMessage()
{
}
