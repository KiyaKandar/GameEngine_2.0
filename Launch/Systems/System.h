#pragma once

#include "Subsystem.h"
#include "../Communication/MessageStorage.h"

#include <vector>
#include <memory>
#include "../Threading/ThreadPool/ThreadPool.h"
#include "Rendering/RenderingSystem.h"

class LetterBox;

class System
{
public:
	System(ThreadPool* threadPool);
	~System();

	void updateNextSystemFrame(const float& deltaTime);

	void addSubsystem(Subsystem* subsystem);
	void addConcurrentSubsystem(Subsystem* subsystem);

	void removeSubsystem(std::string subsystemName);

	std::vector<Subsystem*> getSubSystems();

private:
	MessageStorage messageBuffers;
	LetterBox* letterBox;
	ThreadPool* threadPool;

	std::vector<Subsystem*> subsystems;
	std::vector<Subsystem*> concurrentSubsystems;
};

