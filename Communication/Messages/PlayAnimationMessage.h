#pragma once

#include "../Communication/Message.h"
#include "../../Graphics/Animation/AnimationComponents.h"

#include <string>

struct Node;

class PlayAnimationMessage : public Message
{
public:
	PlayAnimationMessage() : Message("", DUMMY_TYPE) {}
	PlayAnimationMessage(const std::string& desinationName, const std::string& gameObjectID,
		AnimationParams animationParams, AnimationParams transition);
	~PlayAnimationMessage();

	static PlayAnimationMessage Builder(Node* node);

	std::string gameObjectID;
	AnimationParams animationParams;
	AnimationParams transition;

private:
	static AnimationParams ParamsBuilder(Node* node);
	static NodeTransformSpecifier BlockerBuilder(Node* node);
};

