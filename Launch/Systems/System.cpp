#include "System.h"

#include "Communication/DeliverySystem.h"
#include "Communication/LetterBox.h"
#include <iostream>
#include <ctime>

System::System(ThreadPool* threadPool)
{
	letterBox = new LetterBox();
	DeliverySystem::provide(letterBox);
	this->threadPool = threadPool;
}

System::~System()
{
	for (Subsystem* subsystem : subsystems)
	{
		delete (subsystem);
	}
	subsystems.clear();
	for (Subsystem* subsystem : concurrentSubsystems)
	{
		delete (subsystem);
	}
	concurrentSubsystems.clear();
}

void System::updateNextSystemFrame(const float& deltaTime)
{
	
	vector<TaskFuture<void>> updates;

	for (Subsystem* subsystem : concurrentSubsystems)
	{
		updates.push_back(threadPool->submitJob([](float deltaTime, Subsystem* subsystem)
		{
			subsystem->updateSubsystem(deltaTime);
		}, deltaTime, subsystem));
	}

	for (Subsystem* subsystem : subsystems)
	{
		subsystem->updateSubsystem(deltaTime);
	}

	for (auto& task : updates)
	{
		task.Complete();
	}

	auto end = std::clock();

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
std::vector<Subsystem*> System::getSubSystems()
{
	vector<Subsystem*> allSubsystems;
	allSubsystems.insert(std::end(allSubsystems), std::begin(subsystems), std::end(subsystems));
	allSubsystems.insert(std::end(allSubsystems), std::begin(concurrentSubsystems), std::end(concurrentSubsystems));

	return allSubsystems;
}
