#include "SchedulerSystemClock.h"

#include "SubsystemWorkload.h"
#include "SubsystemScheduler.h"
#include "ThreadPackingLayer.h"
#include "Launch/Profiler/SchedulerPerformanceLog.h"
#include "../../Input/Devices/Window.h"

const int REDISTRIBUTE_WORKLOAD_FRAME_DELAY = 2;

SchedulerSystemClock::SchedulerSystemClock(const int activeThreadCount, std::vector<Worker>* workers, Worker* mainThreadWorker,
	std::vector<SubsystemWorkload*>* processes, std::vector<SubsystemWorkload*>* mainThreadProcesses, Window* window)
{
	numThreadsToWaitFor = activeThreadCount;
	numActiveThreads = activeThreadCount;
	syncGeneration = 0;

	this->workers = workers;
	this->mainThreadWorker = mainThreadWorker;

	this->processes = processes;
	this->mainThreadProcesses = mainThreadProcesses;

	this->window = window;
}

SchedulerSystemClock::~SchedulerSystemClock()
{

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
	++numThreadsToWaitFor;
}

void SchedulerSystemClock::UnregisterActiveThread()
{
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

	window->UpdateWindow();

	MarkLaunchStartTime();
	RelaunchThreadsAtThreadBarrier();
}

void SchedulerSystemClock::RescheduleWorkloadIfFrameCountDelayElapsed()
{
	++frameCount;
	if (frameCount == REDISTRIBUTE_WORKLOAD_FRAME_DELAY)
	{
		numThreadsToWaitFor = 0;
		ThreadPackingLayer::DistributeWorkloadAmongWorkerThreads(this, *workers, mainThreadWorker,
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