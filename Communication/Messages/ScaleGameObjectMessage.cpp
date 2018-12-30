#include "ScaleGameObjectMessage.h"

ScaleGameObjectMessage::ScaleGameObjectMessage(const std::string& desinationName, std::string gameObjectID,
	NCLVector3 scale)
	: Message(desinationName, SCALE_GAMEOBJECT)
{
	this->gameObjectID = gameObjectID;
	this->scale = scale;
}

ScaleGameObjectMessage::~ScaleGameObjectMessage()
{
}

ScaleGameObjectMessage ScaleGameObjectMessage::Builder(Node* node)
{
	std::string destination = "";
	std::string object = "";
	NCLVector3 scale;

	for (Node* childNode : node->children)
	{
		if (childNode->nodeType == "destination")
		{
			destination = childNode->value;
		}
		else if (childNode->nodeType == "resource")
		{
			object = childNode->value;
		}
		else if (childNode->nodeType == "scale")
		{
			scale = VectorBuilder::buildVector3(childNode);
		}
	}

	return ScaleGameObjectMessage(destination, object, scale);
}

ScaleGameObjectMessage ScaleGameObjectMessage::TokensToMessage(std::vector<std::string> lineTokens)
{
	std::string nodeDestination = lineTokens[1];
	std::string nodeResourcename = lineTokens[2];

	std::string scaleString = lineTokens[3].substr(6);
	NCLVector3 scale = VectorBuilder::buildVector3(scaleString);

	return ScaleGameObjectMessage(nodeDestination, nodeResourcename, scale);
}
