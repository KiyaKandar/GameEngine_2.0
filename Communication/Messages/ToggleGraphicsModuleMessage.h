#pragma once

#include "../Message.h"

struct Node;

class ToggleGraphicsModuleMessage : public Message
{
public:
	ToggleGraphicsModuleMessage(const std::string& destinationName, const std::string& moduleName, const bool enabled);
	~ToggleGraphicsModuleMessage();

	static ToggleGraphicsModuleMessage builder(Node* node);

	std::string moduleName;
	bool enabled;
};

