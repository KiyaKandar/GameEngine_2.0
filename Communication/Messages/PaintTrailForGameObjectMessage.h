#pragma once

#include "../Message.h"

struct Node;

class PaintTrailForGameObjectMessage : public Message
{
public:
	PaintTrailForGameObjectMessage(const std::string& desinationName, 
		const std::string& resourceName);
	~PaintTrailForGameObjectMessage();

	static PaintTrailForGameObjectMessage builder(Node* node);

	std::string resourceName;
};

