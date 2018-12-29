#pragma once

#include "../Message.h"
#include "../Utilities/Maths/Vector3.h"

class UpdatePositionMessage : public Message
{
public:
	UpdatePositionMessage() : Message("", DUMMY_TYPE) {}
	UpdatePositionMessage(const std::string& desinationName, const std::string& gameObjectID, const NCLVector3& position);
	~UpdatePositionMessage();

	std::string gameObjectID;
	NCLVector3 position;
};

