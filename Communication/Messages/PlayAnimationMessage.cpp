#include "PlayAnimationMessage.h"

#include "../Resource Management/XMLParser.h"

PlayAnimationMessage::PlayAnimationMessage(const std::string & desinationName, std::string gameObjectID,
	std::string animationName, const double lerpToTime)
	: Message(desinationName, PLAY_ANIMATION)
{
	this->gameObjectID = gameObjectID;
	this->animationName = animationName;
	this->lerpToTime = lerpToTime;
}

PlayAnimationMessage::~PlayAnimationMessage()
{
}

PlayAnimationMessage PlayAnimationMessage::builder(Node* node)
{
	std::string destination = "";
	std::string object = "";
	std::string animation = "";
	double lerpTime = 0.0;

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
		else if (childNode->nodeType == "animationName")
		{
			animation = childNode->value;
		}
		else if (childNode->nodeType == "lerpToTime")
		{
			lerpTime = std::stod(childNode->value);
		}
	}

	return PlayAnimationMessage(destination, object, animation, lerpTime);
}
