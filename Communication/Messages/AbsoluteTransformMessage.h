#pragma once

#include "../Message.h"
#include "../Utilities/Maths/Matrix4.h"

struct Node;

class AbsoluteTransformMessage : public Message
{
public:
	AbsoluteTransformMessage() : Message("", DUMMY_TYPE) {}
	AbsoluteTransformMessage(const std::string& destinationName, const std::string& resourceName,
		NCLMatrix4 transform);
	~AbsoluteTransformMessage();

	static AbsoluteTransformMessage builder(Node* node);
	static AbsoluteTransformMessage tokensToMessage(std::vector<std::string> lineTokens);

	NCLMatrix4 transform;
	std::string resourceName;
};

