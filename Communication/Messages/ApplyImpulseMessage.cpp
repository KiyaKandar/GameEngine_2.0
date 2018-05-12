#include "ApplyImpulseMessage.h"

ApplyImpulseMessage::ApplyImpulseMessage(const std::string& desinationName, std::string gameObjectID, bool isRandom,
	NCLVector3 impulse, float xmin, float xmax, float ymin, float ymax,
	float zmin, float zmax) : Message(desinationName, APPLY_IMPULSE)
{
	this->gameObjectID = gameObjectID;
	this->isRandom = isRandom;
	this->impulse = impulse;
	this->xmin = xmin;
	this->xmax = xmax;
	this->ymin = ymin;
	this->ymax = ymax;
	this->zmin = zmin;
	this->zmax = zmax;
}

ApplyImpulseMessage::~ApplyImpulseMessage()
{
}

ApplyImpulseMessage ApplyImpulseMessage::builder(Node* node)
{
	std::string destination = "";
	std::string object = "";
	bool rand = false;
	NCLVector3 impulse;

	float xmin, xmax;
	float ymin, ymax;
	float zmin, zmax;

	for (Node* childNode : node->children)
	{
		if(childNode->nodeType == "destination")
		{
			destination = childNode->value;
		}
		else if (childNode->nodeType == "resource")
		{
			object = childNode->value;
		}
		else if (childNode->nodeType == "isRandom")
		{
			if (childNode->value == "True")
			{
				rand = true;
			}
			else
			{
				rand = false;
			}
		}
		else if (childNode->nodeType == "impulse")
		{
			impulse.x = VectorBuilder::getVectorComponentFromNode(childNode->children[0], &xmin, &xmax);
			impulse.y = VectorBuilder::getVectorComponentFromNode(childNode->children[1], &ymin, &ymax);
			impulse.z = VectorBuilder::getVectorComponentFromNode(childNode->children[2], &zmin, &zmax);
		}
	}

	if(rand)
	{
		return ApplyImpulseMessage(destination, object, rand, impulse, xmin, xmax, ymin, ymax, zmin, zmax);
	}
	return ApplyImpulseMessage(destination, object, rand, impulse);
}

//APPLY_FORCE Physics player0 impulse=5,5,5
ApplyImpulseMessage ApplyImpulseMessage::tokensToMessage(std::vector<std::string> lineTokens)
{
	std::string nodeDestination = lineTokens[1];
	std::string nodeResourcename = lineTokens[2];

	std::string impulseString = lineTokens[3].substr(8);
	NCLVector3 impulse = VectorBuilder::buildVector3(impulseString);

	return ApplyImpulseMessage(nodeDestination, nodeResourcename, false, impulse);
}

