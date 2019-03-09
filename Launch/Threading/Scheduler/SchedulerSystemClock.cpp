#include "SchedulerSystemClock.h"

#include "SubsystemWorkload.h"
#include "SubsystemScheduler.h"

const float frameLengthMS = (1.0f / 60.0f) * 1000.0f;

SchedulerSystemClock::SchedulerSystemClock(const int activeThreadCount)
{
	numThreadsToWaitFor = activeThreadCount;
	numActiveThreads = activeThreadCount;
	syncGeneration = 0;
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


float SchedulerSystemClock::GetLastFrameTime() const
{
	return frameTime;
}

void SchedulerSystemClock::CancelFrameSynchronisation()
{
	launchCondition.notify_all();
}

void SchedulerSystemClock::CompleteFrame()
{
	std::lock_guard<std::mutex> lock(registrationMutex);
	MarkLaunchEndTime();
	SleepUntilNextFrameLaunch();

	syncGeneration++;
	numActiveThreads = numThreadsToWaitFor;
	launchCondition.notify_all();
	MarkLaunchStartTime();
}

void SchedulerSystemClock::SleepUntilNextFrameLaunch()
{
	frameTime = clock.GetTimeTakenForSection();
	const float timeToSleep = frameLengthMS - frameTime;

	if (timeToSleep > FLT_EPSILON)
	{
		Sleep(timeToSleep);
	}
}

void SchedulerSystemClock::MarkLaunchStartTime()
{
	clock.BeginTimedSection();
}

void SchedulerSystemClock::MarkLaunchEndTime()
{
	clock.EndTimedSection();
}