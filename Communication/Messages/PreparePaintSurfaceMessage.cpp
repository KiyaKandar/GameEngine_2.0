#include "PreparePaintSurfaceMessage.h"

#include "../Resource Management/XMLParser.h"

PreparePaintSurfaceMessage::PreparePaintSurfaceMessage(const std::string& desinationName,
	std::vector<std::string> surfaceObjectIdentifiers)
	: Message(desinationName, PREPARE_PAINT_SURFACE)
{
	this->surfaceObjectIdentifiers = surfaceObjectIdentifiers;
}

PreparePaintSurfaceMessage::~PreparePaintSurfaceMessage()
{
}

PreparePaintSurfaceMessage PreparePaintSurfaceMessage::builder(Node* node)
{
	std::string nodeDestination = "";
	std::vector<std::string> nodeSurfaceObjectIdentifiers;

	for (Node* childNode : node->children)
	{
		if (childNode->nodeType == "destination")
		{
			nodeDestination = childNode->value;
		}
		else if (childNode->nodeType == "resource")
		{
			nodeSurfaceObjectIdentifiers.push_back(childNode->value);
		}
	}

	return PreparePaintSurfaceMessage(nodeDestination, nodeSurfaceObjectIdentifiers);
}
