#include "StopSoundMessage.h"

StopSoundMessage::StopSoundMessage(const std::string& desinationName, std::string soundNodeIdentifier) : Message(desinationName, STOP_SOUND)
{
	this->soundNodeIdentifier = soundNodeIdentifier;
}

StopSoundMessage::~StopSoundMessage()
{
}
