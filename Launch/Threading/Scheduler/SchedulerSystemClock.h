#pragma once

#include "GameTimer.h"

#include <mutex>

class SchedulerSystemClock
{
public:
	explicit SchedulerSystemClock(const int activeThreadCount);

	void WaitForSynchronisedLaunch();

	void RegisterActiveThread();
	void UnregisterActiveThread();

	float GetLastFrameTime() const;
	void CancelFrameSynchronisation();

	void MarkLaunchStartTime();
	void MarkLaunchEndTime();

private:
	void CompleteFrame();
	void SleepUntilNextFrameLaunch();

	std::mutex syncMutex;
	std::mutex registrationMutex;

	std::size_t numThreadsToWaitFor;
	std::size_t numActiveThreads;
	std::size_t syncGeneration;
	std::condition_variable launchCondition;

	GameTimer clock;
	float frameTime;
};