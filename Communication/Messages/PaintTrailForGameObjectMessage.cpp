#include "PaintTrailForGameObjectMessage.h"

#include "../Resource Management/XMLParser.h"

PaintTrailForGameObjectMessage::PaintTrailForGameObjectMessage(const std::string& desinationName,
	const std::string& resourceName)
	: Message(desinationName, PAINT_TRAIL_FOR_GAMEOBJECT)
{
	this->resourceName = resourceName;
}

PaintTrailForGameObjectMessage::~PaintTrailForGameObjectMessage()
{
}

PaintTrailForGameObjectMessage PaintTrailForGameObjectMessage::Builder(Node * node)
{
	std::string nodeDestination = "";
	std::string nodeResourceName = "";

	for (Node* childNode : node->children)
	{
		if (childNode->nodeType == "destination")
		{
			nodeDestination = childNode->value;
		}
		else if (childNode->nodeType == "resource")
		{
			nodeResourceName = childNode->value;
		}
	}

	return PaintTrailForGameObjectMessage(nodeDestination, nodeResourceName);
}
