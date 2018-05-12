#include "Subsystem.h"
#include "../Utilities/GameTimer.h"

Subsystem::Subsystem(std::string subsystemName)
{
	this->subsystemName = subsystemName;
	DeliverySystem::getPostman()->addDeliveryPoint(subsystemName);
	timer = new GameTimer();
	timer->addChildTimer("Message Processing");
}

Subsystem::~Subsystem()
{
	delete timer;
}

void Subsystem::updateSubsystem(const float& deltaTime)
{
	processMessages();
	updateNextFrame(deltaTime);
}

void Subsystem::processMessages()
{
	timer->beginChildTimedSection("Message Processing");
	incomingMessages.processMessagesInBuffer();
	timer->endChildTimedSection("Message Processing");
}

GameTimer* Subsystem::getTimer()
{
	return timer;
}

std::string Subsystem::getSubsystemName()
{
	return subsystemName;
}

