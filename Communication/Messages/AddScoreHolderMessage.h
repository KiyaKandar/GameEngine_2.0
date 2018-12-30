#pragma once

#include "../Message.h"
#include "../Utilities/Maths/Vector4.h"

struct Node;

class AddScoreHolderMessage : public Message
{
public:
	AddScoreHolderMessage() : Message("", DUMMY_TYPE) {}
	AddScoreHolderMessage(const std::string& desinationName, const std::string& name);
	~AddScoreHolderMessage();

	static AddScoreHolderMessage Builder(Node* node);
	static AddScoreHolderMessage TokensToMessage(std::vector<std::string> lineTokens);

	std::string name;
};

