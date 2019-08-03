#include "SchedulerSystemClock.h"

#include "SubsystemWorkload.h"
#include "SubsystemScheduler.h"
#include "ThreadPackingLayer.h"
#include "Launch/Profiler/SchedulerPerformanceLog.h"

const int REDISTRIBUTE_WORKLOAD_FRAME_DELAY = 2;

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
			CompleteFrameAsLastFinishedThread();
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

void SchedulerSystemClock::CompleteFrameAsLastFinishedThread()
{
	std::lock_guard<std::mutex> lock(registrationMutex);

	RescheduleWorkloadIfFrameCountDelayElapsed();
	MarkLaunchEndTime();
	MarkLaunchStartTime();
	RelaunchThreadsAtThreadBarrier();
}

void SchedulerSystemClock::RescheduleWorkloadIfFrameCountDelayElapsed()
{
	++frameCount;
	if (frameCount == REDISTRIBUTE_WORKLOAD_FRAME_DELAY)
	{
		ThreadPackingLayer::DistributeWorkloadAmongWorkerThreads(*workers, mainThreadWorker,
			*processes, *mainThreadProcesses);
		frameCount = 0;
	}
}

void SchedulerSystemClock::RelaunchThreadsAtThreadBarrier()
{
	syncGeneration++;
	numActiveThreads = numThreadsToWaitFor;
	launchCondition.notify_all();

	for (Worker& worker : *workers)
	{
		worker.hasWork.notify_one();
	}
}

void SchedulerSystemClock::MarkLaunchStartTime()
{
	clock.BeginTimedSection();
}

void SchedulerSystemClock::MarkLaunchEndTime()
{
	clock.EndTimedSection();

	SchedulerPerformanceLog::Retrieve()->AddElapsedFrameTime(clock.GetTimeTakenForSection());
}