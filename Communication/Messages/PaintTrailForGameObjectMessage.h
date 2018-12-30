#pragma once

#include "../Message.h"

struct Node;

class PaintTrailForGameObjectMessage : public Message
{
public:
	PaintTrailForGameObjectMessage() : Message("", DUMMY_TYPE) {}
	PaintTrailForGameObjectMessage(const std::string& desinationName, 
		const std::string& resourceName);
	~PaintTrailForGameObjectMessage();

	static PaintTrailForGameObjectMessage Builder(Node* node);

	std::string resourceName;
};

