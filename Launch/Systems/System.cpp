#include "System.h"

#include "Communication/DeliverySystem.h"
#include "Communication/LetterBox.h"
#include "../Profiler/Profiler.h"
#include <iostream>
#include <ctime>

System::System(ThreadPool* threadPool)
{
	letterBox = new LetterBox();
	DeliverySystem::provide(letterBox);
	this->threadPool = threadPool;
	timer = new GameTimer();
	running = false;
}

System::~System()
{
	for (Subsystem* subsystem : subsystems)
	{
		delete (subsystem);
	}

	for (Subsystem* subsystem : concurrentSubsystems)
	{
		delete (subsystem);
	}

	subsystems.clear();
	concurrentSubsystems.clear();

	delete timer;
}

void System::updateNextSystemFrame()
{
	timer->beginTimedSection();

	for (Subsystem* subsystem : subsystems)
	{
		subsystem->updateSubsystem();
	}

	DeliverySystem::getPostman()->clearAllMessages();
	DeliverySystem::getPostman()->deliverAllMessages();

	timer->endTimedSection();
}

void System::StartConcurrentSubsystems()
{
	running = true;

	for (Subsystem* subsystem : concurrentSubsystems)
	{
		updates.push_back(threadPool->submitJob([](const bool* running, Subsystem* subsystem)
		{
			while (*running)
			{
				subsystem->updateSubsystem();
				DeliverySystem::getPostman()->clearAllMessages();
				DeliverySystem::getPostman()->deliverAllMessages();
			}
		}, &running, subsystem));
	}
}

void System::SynchroniseAndStopConcurrentSubsystems()
{
	running = false;

	for (auto& task : updates)
	{
		task.Complete();
	}
}

void System::addSubsystem(Subsystem* subsystem)
{
	subsystems.push_back(subsystem);
}

void System::addConcurrentSubsystem(Subsystem* subsystem)
{
	concurrentSubsystems.push_back(subsystem);
}

void System::removeSubsystem(std::string subsystemName)
{
	bool erased = false;

	for (auto concurrentSubsystemIterator = concurrentSubsystems.begin(); concurrentSubsystemIterator != concurrentSubsystems.end(); ++concurrentSubsystemIterator)
	{
		if ((*concurrentSubsystemIterator)->getSubsystemName() == subsystemName)
		{
			concurrentSubsystems.erase(concurrentSubsystemIterator);
			erased = true;
			break;
		}
	}

	if (!erased)
	{
		for (auto subsystemIterator = subsystems.begin(); subsystemIterator != subsystems.end(); ++subsystemIterator)
		{
			if ((*subsystemIterator)->getSubsystemName() == subsystemName)
			{
				subsystems.erase(subsystemIterator);
				break;
			}
		}
	}
}

void System::RegisterWithProfiler(Profiler* profiler)
{
	profiler->addSubsystemTimer("System Frame", timer);
}

std::vector<Subsystem*> System::getSubSystems()
{
	vector<Subsystem*> allSubsystems;
	allSubsystems.insert(std::end(allSubsystems), std::begin(subsystems), std::end(subsystems));
	allSubsystems.insert(std::end(allSubsystems), std::begin(concurrentSubsystems), std::end(concurrentSubsystems));

	return allSubsystems;
}
