#pragma once

#include "../Communication/Message.h"
#include "../Audio/Sound.h"
#include "../Audio/SoundNode.h"

class PlayMovingSoundMessage : public Message
{
public:
	PlayMovingSoundMessage() : Message("", DUMMY_TYPE) {}
	PlayMovingSoundMessage(const std::string& desinationName, NCLVector3 *position,
		SoundPriority priority, float volume, float radius, float pitch, bool isLooping, bool isGlobal,
		std::string soundID, std::string soundNodeIdentifier, std::string gameObjectID);
	~PlayMovingSoundMessage();

	static PlayMovingSoundMessage builder(Node* node);

	NCLVector3 *position;
	SoundPriority priority;
	float volume;
	float radius;
	float pitch;
	bool isLooping;
	bool isGlobal;
	std::string soundID;
	std::string soundNodeIdentifier;
	std::string gameObjectID;
};

