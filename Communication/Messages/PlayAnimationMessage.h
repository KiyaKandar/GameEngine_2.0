#pragma once

#include "../Communication/Message.h"

#include <string>

struct Node;

class PlayAnimationMessage : public Message
{
public:
	PlayAnimationMessage(const std::string& desinationName, std::string gameObjectID,
		std::string animationName, const double lerpToTime);
	~PlayAnimationMessage();

	static PlayAnimationMessage builder(Node* node);

	std::string gameObjectID;
	std::string animationName;
	double lerpToTime;
};

