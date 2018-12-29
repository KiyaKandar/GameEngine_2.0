#pragma once

#include "../Communication/Message.h"
#include "../Audio/Sound.h"
#include "../Audio/SoundNode.h"

class PlaySoundMessage : public Message
{
public:
	PlaySoundMessage() : Message("", DUMMY_TYPE) {}
	PlaySoundMessage(const std::string& desinationName, NCLVector3 position,
		 SoundPriority priority, float volume, float radius, float pitch, bool isLooping,
		std::string soundID, std::string soundNodeIdentifier);
	~PlaySoundMessage();

	static PlaySoundMessage builder(Node* node);

	NCLVector3 position;
	SoundPriority priority;
	float volume;
	float radius;
	float pitch;
	bool isLooping;
	std::string soundID;
	std::string soundNodeIdentifier;
};
