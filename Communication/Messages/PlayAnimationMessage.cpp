#include "PlayAnimationMessage.h"

#include "../Resource Management/XMLParser.h"

PlayAnimationMessage::PlayAnimationMessage(const std::string& desinationName, const std::string& gameObjectID,
	AnimationParams animationParams, AnimationParams transition)
	: Message(desinationName, PLAY_ANIMATION)
{
	this->gameObjectID = gameObjectID;
	this->animationParams = animationParams;
	this->transition = transition;
}

PlayAnimationMessage::~PlayAnimationMessage()
{
}

PlayAnimationMessage PlayAnimationMessage::builder(Node* node)
{
	std::string destination = "";
	std::string object = "";
	AnimationParams animationParams;
	AnimationParams transition;

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
		else if (childNode->nodeType == "animation")
		{
			animationParams = paramsBuilder(childNode);
		}
		else if (childNode->nodeType == "transition")
		{
			transition = paramsBuilder(childNode);
		}
	}

	return PlayAnimationMessage(destination, object, animationParams, transition);
}

AnimationParams PlayAnimationMessage::paramsBuilder(Node * node)
{
	AnimationParams params;

	for (Node* childNode : node->children)
	{
		if (childNode->nodeType == "animationName")
		{
			params.animationName = childNode->value;
		}
		else if (childNode->nodeType == "lerpToTime")
		{
			params.lerpToTime = std::stod(childNode->value);
		}
		else if (childNode->nodeType == "loop")
		{
			params.loop = childNode->value == "True" ? true : false;
		}
		else if (childNode->nodeType == "nodeTransformBlocker")
		{
			params.transformBlocker = blockerBuilder(childNode);
		}
	}

	return params;
}

NodeTransformBlocker PlayAnimationMessage::blockerBuilder(Node * node)
{
	NodeTransformBlocker transformBlocker;

	for (Node* childNode : node->children)
	{
		if (childNode->nodeType == "nodeName")
		{
			transformBlocker.nodeName = childNode->value;
		}
		else if (childNode->nodeType == "rotation")
		{
			transformBlocker.blockedComponents.blockRotation = childNode->value == "True" ? true : false;
		}
		else if (childNode->nodeType == "translation")
		{
			transformBlocker.blockedComponents.blockTranslation = childNode->value == "True" ? true : false;
		}
		else if (childNode->nodeType == "scale")
		{
			transformBlocker.blockedComponents.blockScale = childNode->value == "True" ? true : false;
		}
	}
	return transformBlocker;
}
