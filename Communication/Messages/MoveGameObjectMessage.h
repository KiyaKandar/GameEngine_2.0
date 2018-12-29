#pragma once

#include "../Communication/Message.h"
#include "../Utilities/Maths/Vector3.h"
#include <string>

struct Node;

class MoveGameObjectMessage : public Message
{
public:
	MoveGameObjectMessage() : Message("", DUMMY_TYPE) {}
	MoveGameObjectMessage(const std::string& desinationName, std::string gameObjectID,
		NCLVector3 position);
	~MoveGameObjectMessage();

	static MoveGameObjectMessage builder(Node* node);
	static MoveGameObjectMessage tokensToMessage(std::vector<std::string> lineTokens);

	std::string gameObjectID;
	NCLVector3 position;
};

