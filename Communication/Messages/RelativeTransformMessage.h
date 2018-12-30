#pragma once

#include "../Message.h"
#include "../Utilities/Maths/Matrix4.h"

struct Node;

class RelativeTransformMessage : public Message
{
public:
	RelativeTransformMessage() : Message("", DUMMY_TYPE) {}
	RelativeTransformMessage(const std::string& destinationName, const std::string& resourceName, 
		NCLMatrix4 transform);
	~RelativeTransformMessage();

	static RelativeTransformMessage Builder(Node* node);
	static RelativeTransformMessage TokensToMessage(std::vector<std::string> lineTokens);

	NCLMatrix4 transform;
	std::string resourceName;
};

