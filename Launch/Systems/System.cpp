#include "System.h"

#include "Communication/DeliverySystem.h"
#include "Communication/LetterBox.h"
#include "../Profiler/Profiler.h"
#include <iostream>
#include <ctime>
#include "Launch/Game/GameLoop.h"

atomic_bool System::stop = false;

System::System()
{
	timer = new GameTimer();
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

	delete timer;
}

void System::UpdateNextSystemFrame()
{
	timer->BeginTimedSection();

	for (Subsystem* subsystem : subsystems)
	{
		subsystem->UpdateSubsystem();
	}

	timer->EndTimedSection();
}

void System::StartConcurrentSubsystems(GameLoop* game)
{
	stop = false;

	for (Subsystem*& subsystem : concurrentSubsystems)
	{
		ProcessScheduler::Retrieve()->RegisterProcess(std::bind(&Subsystem::UpdateSubsystem, subsystem), subsystem->GetSubsystemName());
	}

	ProcessScheduler::Retrieve()->AttachMainThreadProcess(std::bind(&GameLoop::ExecuteGameLoop, game), "Renderer");

	ProcessScheduler::Retrieve()->BeginWorkerProcesses();
}

void System::SynchroniseAndStopConcurrentSubsystems()
{
	stop = true;
	ProcessScheduler::Retrieve()->CompleteWorkerProcesses();
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

std::vector<Subsystem*> System::GetSubSystems()
{
	vector<Subsystem*> allSubsystems;
	allSubsystems.insert(std::end(allSubsystems), std::begin(subsystems), std::end(subsystems));
	allSubsystems.insert(std::end(allSubsystems), std::begin(concurrentSubsystems), std::end(concurrentSubsystems));

	return allSubsystems;
}
