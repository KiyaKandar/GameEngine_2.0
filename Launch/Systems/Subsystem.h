#pragma once

#include "../Communication/MessageProcessor.h"
#include "../Communication/DeliverySystem.h"

class GameTimer;

class Subsystem
{
public:

	Subsystem(std::string subsystemName);

	virtual ~Subsystem();

	void UpdateSubsystem();
	virtual void UpdateNextFrame(const float& deltaTime = 0.0f) = 0;

	void ProcessMessages();

	GameTimer* GetTimer() const;
	std::string GetSubsystemName() const;

protected:
	std::string subsystemName;
	MessageProcessor incomingMessages;
	GameTimer* timer;
	GameTimer* frameTimer;
};
