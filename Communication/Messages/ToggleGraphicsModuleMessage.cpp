#include "ToggleGraphicsModuleMessage.h"

#include "../Resource Management/XMLParser.h"

ToggleGraphicsModuleMessage::ToggleGraphicsModuleMessage(const std::string& destinationName, 
	const std::string& moduleName, const bool enabled)
	: Message(destinationName, TOGGLE_GRAPHICS_MODULE)
{
	this->enabled = enabled;
	this->moduleName = moduleName;
}

ToggleGraphicsModuleMessage::~ToggleGraphicsModuleMessage()
{
}

ToggleGraphicsModuleMessage ToggleGraphicsModuleMessage::Builder(Node* node)
{
	std::string nodeDestination = "";
	std::string nodeModuleName = "";
	bool nodeEnabled = false;

	for (Node* childNode : node->children)
	{
		if (childNode->nodeType == "destination")
		{
			nodeDestination = childNode->value;
		}
		else if (childNode->nodeType == "moduleName")
		{
			nodeModuleName = childNode->value;
		}
		else if (childNode->nodeType == "enabled")
		{
			nodeEnabled = childNode->value == "True";
		}
	}

	return ToggleGraphicsModuleMessage(nodeDestination, nodeModuleName, nodeEnabled);
}
