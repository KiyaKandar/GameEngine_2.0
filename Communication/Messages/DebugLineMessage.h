#pragma once
#include "../Communication/Message.h"
#include <string>
#include "../Utilities/Maths/Vector3.h"

class DebugLineMessage : public Message
{
public:
	DebugLineMessage(const std::string &destinationName, NCLVector3 from,
		NCLVector3 to, NCLVector3 colour);
	~DebugLineMessage();

	NCLVector3 from;
	NCLVector3 to;
	NCLVector3 colour;
};

