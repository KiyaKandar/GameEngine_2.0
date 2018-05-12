#pragma once
#include "../Communication/Message.h"
#include <string>
#include "../Utilities/Maths/Vector3.h"

class DebugSphereMessage : public Message
{
public:
	DebugSphereMessage(const std::string &destinationName, NCLVector3 position,
		float radius, NCLVector3 colour);
	~DebugSphereMessage();

	NCLVector3 position;
	NCLVector3 colour;
	float radius;
};

