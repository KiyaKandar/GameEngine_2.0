#include "PlayMovingSoundMessage.h"

PlayMovingSoundMessage::PlayMovingSoundMessage(const std::string& destinationName, NCLVector3 *position,
	SoundPriority priority, float volume, float radius,
	float pitch, bool isLooping, bool isGlobal, std::string soundID, std::string soundNodeIdentifier, std::string gameObjectID)
	: Message(destinationName, MOVING_SOUND)
{
	this->position = position;
	this->priority = priority;
	this->volume = volume;
	this->radius = radius;
	this->pitch = pitch;
	this->isLooping = isLooping;
	this->soundID = soundID;
	this->soundNodeIdentifier = soundNodeIdentifier;
	this->isGlobal = isGlobal;
	this->gameObjectID = gameObjectID;
}

PlayMovingSoundMessage::~PlayMovingSoundMessage()
{
}

PlayMovingSoundMessage PlayMovingSoundMessage::builder(Node* node)
{
	std::string destination = "";
	NCLVector3 position;
	SoundPriority priority;
	float volume = 0.0f;
	float radius = 0.0f;
	float pitch = 0.0f;
	bool isLooping = false;
	bool isGlobal = false;
	std::string soundID = "";
	std::string soundNodeIdentifier = "";
	std::string gameObjectID = "";

	for (Node* childNode : node->children)
	{
		if (childNode->nodeType == "destination")
		{
			destination = childNode->value;
		}
		else if (childNode->nodeType == "priority")
		{
			if (childNode->value == "SOUNDPRIORITY_LOW")
			{
				priority = SOUNDPRIORITY_LOW;
			}
			else if (childNode->value == "SOUNDPRIORITY_MEDIUM")
			{
				priority = SOUNDPRIORITY_MEDIUM;
			}
			else if (childNode->value == "SOUNDPRIORITY_HIGH")
			{
				priority = SOUNDPRIORITY_HIGH;
			}
			else if (childNode->value == "SOUNDPRIORITY_ALWAYS")
			{
				priority = SOUNDPRIORITY_ALWAYS;
			}
		}
		else if (childNode->nodeType == "volume")
		{
			volume = std::stof(childNode->value);
		}
		else if (childNode->nodeType == "radius")
		{
			radius = std::stof(childNode->value);
		}
		else if (childNode->nodeType == "pitch")
		{
			pitch = std::stof(childNode->value);
		}
		else if (childNode->nodeType == "isLooping")
		{
			if (childNode->value == "true")
			{
				isLooping = true;
			}
			else
			{
				isLooping = false;
			}
		}
		else if (childNode->nodeType == "isGlobal")
		{
			if (childNode->value == "true")
			{
				isGlobal = true;
			}
			else
			{
				isGlobal = false;
			}
		}
		else if (childNode->nodeType == "sound")
		{
			soundID = childNode->value;
		}
		else if (childNode->nodeType == "soundNodeID")
		{
			soundNodeIdentifier = childNode->value;
		}
		else if (childNode->nodeType == "gameObjectID")
		{
			gameObjectID = childNode->value;
		}
	}
	return PlayMovingSoundMessage(destination, &position, priority, volume, radius, pitch, isLooping, isGlobal, soundID, soundNodeIdentifier, gameObjectID);
}

