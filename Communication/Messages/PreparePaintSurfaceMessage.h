#pragma once

#include "../Message.h"

struct Node;

class PreparePaintSurfaceMessage : public Message
{
public:
	PreparePaintSurfaceMessage() : Message("", DUMMY_TYPE) {}
	PreparePaintSurfaceMessage(const std::string& desinationName, std::vector<std::string> surfaceObjectIdentifiers);
	~PreparePaintSurfaceMessage();

	static PreparePaintSurfaceMessage Builder(Node* node);

	std::vector<std::string> surfaceObjectIdentifiers;
};

