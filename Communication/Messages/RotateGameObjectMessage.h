#pragma once

#include "../Communication/Message.h"
#include "../Utilities/Maths/Vector4.h"
#include <string>

struct Node;

class RotateGameObjectMessage : public Message
{
public:
	RotateGameObjectMessage() : Message("", DUMMY_TYPE) {}
	RotateGameObjectMessage(const std::string& desinationName, std::string gameObjectID,
		NCLVector4 rotation, bool relative = false);
	~RotateGameObjectMessage();

	static RotateGameObjectMessage builder(Node* node);
	static RotateGameObjectMessage tokensToMessage(std::vector<std::string> lineTokens);

	bool relative;
	std::string gameObjectID;
	NCLVector4 rotation;
};