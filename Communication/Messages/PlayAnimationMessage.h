#pragma once

#include "../Communication/Message.h"

#include <string>

struct Node;

struct AnimationParams
{
	std::string animationName = "";
	double lerpToTime = 0.0;
	bool loop = false;
	std::string nodeToBlock = "";
};

class PlayAnimationMessage : public Message
{
public:
	PlayAnimationMessage(const std::string& desinationName, const std::string& gameObjectID,
		AnimationParams animationParams, AnimationParams transition);
	~PlayAnimationMessage();

	static PlayAnimationMessage builder(Node* node);

	std::string gameObjectID;
	AnimationParams animationParams;
	AnimationParams transition;

private:
	static AnimationParams paramsBuilder(Node* node);
};

