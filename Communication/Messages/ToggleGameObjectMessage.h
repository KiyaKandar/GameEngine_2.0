#pragma once

#include "../Communication/Message.h"
#include "../Utilities/Maths/Vector4.h"
#include <string>

struct Node;

class ToggleGameObjectMessage : public Message
{
public:
	ToggleGameObjectMessage(const std::string& destinationName, std::string gameObjectID,
		bool isEnabled);
	~ToggleGameObjectMessage();

	static ToggleGameObjectMessage builder(Node* node);
	static ToggleGameObjectMessage tokensToMessage(std::vector<std::string> lineTokens);
	static bool ToggleGameObjectMessage::stob(std::string str);
	std::string gameObjectID;
	bool isEnabled;
};

