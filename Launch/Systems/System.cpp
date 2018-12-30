#include "System.h"

#include "Communication/DeliverySystem.h"
#include "Communication/LetterBox.h"
#include "../Profiler/Profiler.h"
#include <iostream>
#include <ctime>

System::System(ThreadPool* threadPool)
{
	letterBox = new LetterBox();
	DeliverySystem::Provide(letterBox);
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

void System::UpdateNextSystemFrame()
{
	timer->beginTimedSection();

	for (Subsystem* subsystem : subsystems)
	{
		subsystem->UpdateSubsystem();
	}

	DeliverySystem::GetPostman()->ClearAllMessages();
	DeliverySystem::GetPostman()->DeliverAllMessages();

	timer->endTimedSection();
}

void System::StartConcurrentSubsystems()
{
	running = true;

	for (Subsystem* subsystem : concurrentSubsystems)
	{
		updates.push_back(threadPool->SubmitJob([](const bool* running, Subsystem* subsystem)
		{
			while (*running)
			{
				subsystem->UpdateSubsystem();
				DeliverySystem::GetPostman()->ClearAllMessages();
				DeliverySystem::GetPostman()->DeliverAllMessages();
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

void System::AddSubsystem(Subsystem* subsystem)
{
	subsystems.push_back(subsystem);
}

void System::AddConcurrentSubsystem(Subsystem* subsystem)
{
	concurrentSubsystems.push_back(subsystem);
}

void System::RemoveSubsystem(std::string subsystemName)
{
	bool erased = false;

	for (auto concurrentSubsystemIterator = concurrentSubsystems.begin(); concurrentSubsystemIterator !=
	     concurrentSubsystems.end(); ++concurrentSubsystemIterator)
	{
		if ((*concurrentSubsystemIterator)->GetSubsystemName() == subsystemName)
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
			if ((*subsystemIterator)->GetSubsystemName() == subsystemName)
			{
				subsystems.erase(subsystemIterator);
				break;
			}
		}
	}
}

void System::RegisterWithProfiler(Profiler* profiler) const
{
	profiler->AddSubsystemTimer("System Frame", timer);
}

std::vector<Subsystem*> System::GetSubSystems()
{
	vector<Subsystem*> allSubsystems;
	allSubsystems.insert(std::end(allSubsystems), std::begin(subsystems), std::end(subsystems));
	allSubsystems.insert(std::end(allSubsystems), std::begin(concurrentSubsystems), std::end(concurrentSubsystems));

	return allSubsystems;
}
