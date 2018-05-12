#pragma once

#include "../Message.h"
#include "../Utilities/Maths/Matrix4.h"

struct Node;

class RelativeTransformMessage : public Message
{
public:
	RelativeTransformMessage(const std::string& destinationName, const std::string& resourceName, 
		NCLMatrix4 transform);
	~RelativeTransformMessage();

	static RelativeTransformMessage builder(Node* node);
	static RelativeTransformMessage tokensToMessage(std::vector<std::string> lineTokens);

	NCLMatrix4 transform;
	std::string resourceName;
};

