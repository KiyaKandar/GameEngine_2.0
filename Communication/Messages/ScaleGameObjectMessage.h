#pragma once

#include "../Communication/Message.h"
#include "../Utilities/Maths/Vector3.h"
#include <string>

struct Node;

class ScaleGameObjectMessage : public Message
{
public:
	ScaleGameObjectMessage(const std::string& desinationName, std::string gameObjectID,
		NCLVector3 position);
	~ScaleGameObjectMessage();

	static ScaleGameObjectMessage builder(Node* node);
	static ScaleGameObjectMessage tokensToMessage(std::vector<std::string> lineTokens);

	std::string gameObjectID;
	NCLVector3 scale;
};

