#include "Subsystem.h"
#include "../Utilities/GameTimer.h"
#include "System.h"

Subsystem::Subsystem(std::string subsystemName)
{
	this->subsystemName = subsystemName;
	DeliverySystem::GetPostman()->AddDeliveryPoint(subsystemName);
	timer = new GameTimer();
	frameTimer = new GameTimer();
	timer->AddChildTimer("Message Processing");
}

Subsystem::~Subsystem()
{
	delete timer;
}

void Subsystem::UpdateSubsystem()
{
	ProcessMessages();
	UpdateNextFrame(frameTimer->GetTimeSinceLastRetrieval());
}

void Subsystem::PersistentlyUpdateSubsystem()
{
	while (!System::stop)
	{
		ProcessMessages();
		UpdateNextFrame(frameTimer->GetTimeSinceLastRetrieval());

		DeliverySystem::GetPostman()->ClearAllMessages();
		DeliverySystem::GetPostman()->DeliverAllMessages();
	}
}

void Subsystem::ProcessMessages()
{
	timer->BeginChildTimedSection("Message Processing");
	incomingMessages.ProcessMessagesInBuffer();
	timer->EndChildTimedSection("Message Processing");
}

GameTimer* Subsystem::GetTimer() const
{
	return timer;
}

std::string Subsystem::GetSubsystemName() const
{
	return subsystemName;
}
