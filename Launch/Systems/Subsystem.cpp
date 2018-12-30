#include "Subsystem.h"
#include "../Utilities/GameTimer.h"

Subsystem::Subsystem(std::string subsystemName)
{
	this->subsystemName = subsystemName;
	DeliverySystem::GetPostman()->AddDeliveryPoint(subsystemName);
	timer = new GameTimer();
	frameTimer = new GameTimer();
	timer->addChildTimer("Message Processing");
}

Subsystem::~Subsystem()
{
	delete timer;
}

void Subsystem::UpdateSubsystem()
{
	ProcessMessages();
	UpdateNextFrame(frameTimer->getTimeSinceLastRetrieval());
}

void Subsystem::ProcessMessages()
{
	timer->beginChildTimedSection("Message Processing");
	incomingMessages.ProcessMessagesInBuffer();
	timer->endChildTimedSection("Message Processing");
}

GameTimer* Subsystem::GetTimer() const
{
	return timer;
}

std::string Subsystem::GetSubsystemName() const
{
	return subsystemName;
}
