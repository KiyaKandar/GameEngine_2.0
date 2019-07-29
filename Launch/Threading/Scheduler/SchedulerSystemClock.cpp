#include "SchedulerSystemClock.h"

#include "SubsystemWorkload.h"
#include "SubsystemScheduler.h"
#include "ThreadPackingLayer.h"

const float frameLengthMS = (1.0f / 60.0f) * 1000.0f;

SchedulerSystemClock::SchedulerSystemClock(const int activeThreadCount, std::vector<Worker>* workers, Worker* mainThreadWorker,
	std::vector<SubsystemWorkload*>* processes, std::vector<SubsystemWorkload*>* mainThreadProcesses)
{
	numThreadsToWaitFor = activeThreadCount;
	numActiveThreads = activeThreadCount;
	syncGeneration = 0;

	this->workers = workers;
	this->mainThreadWorker = mainThreadWorker;

	this->processes = processes;
	this->mainThreadProcesses = mainThreadProcesses;
}

void SchedulerSystemClock::WaitForSynchronisedLaunch()
{
	if (SubsystemScheduler::workersRunning)
	{
		std::unique_lock<std::mutex> lLock{ syncMutex };
		auto localGeneration = syncGeneration;
		if (!--numActiveThreads)
		{
			CompleteFrame();
		}
		else
		{
			launchCondition.wait(lLock, [this, localGeneration] { return localGeneration != syncGeneration || !SubsystemScheduler::workersRunning; });
		}
	}
}

void SchedulerSystemClock::RegisterActiveThread()
{
	std::lock_guard<std::mutex> lock(registrationMutex);
	++numThreadsToWaitFor;
}

void SchedulerSystemClock::UnregisterActiveThread()
{
	std::lock_guard<std::mutex> lock(registrationMutex);
	--numThreadsToWaitFor;
}

GameTimer* SchedulerSystemClock::GetClockTimer()
{
	return &clock;
}

void SchedulerSystemClock::CancelFrameSynchronisation()
{
	launchCondition.notify_all();
}

void SchedulerSystemClock::CompleteFrame()
{
	std::lock_guard<std::mutex> lock(registrationMutex);
	MarkLaunchEndTime();

	++frameCount;
	if (frameCount == 2)
	{
		ThreadPackingLayer::DistributeWorkloadAmongWorkerThreads(*workers, mainThreadWorker,
			*processes, *mainThreadProcesses);
		frameCount = 0;
	}

	syncGeneration++;
	numActiveThreads = numThreadsToWaitFor;
	launchCondition.notify_all();

	for (Worker& worker : *workers)
	{
		worker.hasWork.notify_one();
	}

	MarkLaunchStartTime();
}

void SchedulerSystemClock::SleepUntilNextFrameLaunch()
{
	frameTime = clock.GetTimeTakenForSection();
	const float timeToSleep = CalculatelargestTimeTakenForWorker() - frameTime;

	if (timeToSleep > FLT_EPSILON)
	{
		Sleep(timeToSleep);
	}
}

float SchedulerSystemClock::CalculatelargestTimeTakenForWorker() const
{
	float maxWorkload = mainThreadWorker->currentWorkloadSize;
	for (int i = 0; i < workers->size(); ++i)
	{
		maxWorkload = max(maxWorkload, (*workers)[i].currentWorkloadSize);
	}

	return maxWorkload;
}

void SchedulerSystemClock::MarkLaunchStartTime()
{
	clock.BeginTimedSection();
}

void SchedulerSystemClock::MarkLaunchEndTime()
{
	clock.EndTimedSection();
}