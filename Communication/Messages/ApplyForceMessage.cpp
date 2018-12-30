#include "ApplyForceMessage.h"

ApplyForceMessage::ApplyForceMessage(const std::string& desinationName, const std::string& gameObjectID, bool isRandom,
	const NCLVector3 force, float xmin, float xmax, float ymin, float ymax,
	float zmin, float zmax) : Message(desinationName, APPLY_FORCE)
{
	this->gameObjectID = gameObjectID;
	this->isRandom = isRandom;
	this->force = force;
	this->xmin = xmin;
	this->xmax = xmax;
	this->ymin = ymin;
	this->ymax = ymax;
	this->zmin = zmin;
	this->zmax = zmax;
}

ApplyForceMessage::~ApplyForceMessage()
{
}

ApplyForceMessage ApplyForceMessage::Builder(Node* node)
{
	std::string destination = "";
	std::string object = "";
	bool rand = false;
	NCLVector3 force;

	float xmin, xmax;
	float ymin, ymax;
	float zmin, zmax;

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
		else if (childNode->nodeType == "force")
		{
			force.x = VectorBuilder::GetVectorComponentFromNode(childNode->children[0], &xmin, &xmax);
			force.y = VectorBuilder::GetVectorComponentFromNode(childNode->children[1], &ymin, &ymax);
			force.z = VectorBuilder::GetVectorComponentFromNode(childNode->children[2], &zmin, &zmax);
		}
	}

	if (rand)
	{
		return ApplyForceMessage(destination, object, rand, force, xmin, xmax, ymin, ymax, zmin, zmax);
	}

	return ApplyForceMessage(destination, object, rand, force);
}

//APPLY_FORCE Physics player0 force=5,5,5
ApplyForceMessage ApplyForceMessage::TokensToMessage(std::vector<std::string> lineTokens)
{
	std::string nodeDestination = lineTokens[1];
	std::string nodeResourcename = lineTokens[2];

	std::string forceString = lineTokens[3].substr(6);
	NCLVector3 force = VectorBuilder::BuildVector3(forceString);

	return ApplyForceMessage(nodeDestination, nodeResourcename, false, force);
}
