#include "RotateGameObjectMessage.h"

RotateGameObjectMessage::RotateGameObjectMessage(const std::string& desinationName, std::string gameObjectID,
	NCLVector4 rotation, bool relative)
	: Message(desinationName, ROTATE_GAMEOBJECT)
{
	this->relative = relative;
	this->gameObjectID = gameObjectID;
	this->rotation = rotation;
}

RotateGameObjectMessage::~RotateGameObjectMessage()
{
}

RotateGameObjectMessage RotateGameObjectMessage::builder(Node* node)
{
	std::string destination = "";
	std::string object = "";
	NCLVector4 rotation;
	bool nodeRelative = false;

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
		else if (childNode->nodeType == "rotation")
		{
			rotation = VectorBuilder::buildVector4(childNode);
		}
		else if (childNode->nodeType == "relative")
		{
			nodeRelative = childNode->value == "True";
		}
	}

	return RotateGameObjectMessage(destination, object, rotation, nodeRelative);
}

RotateGameObjectMessage RotateGameObjectMessage::tokensToMessage(std::vector<std::string> lineTokens)
{
	std::string nodeDestination = lineTokens[1];
	std::string nodeResourcename = lineTokens[2];

	std::string rotateString = lineTokens[3].substr(7);
	NCLVector4 rotate = VectorBuilder::buildVector4(rotateString);

	return RotateGameObjectMessage(nodeDestination, nodeResourcename, rotate);
}