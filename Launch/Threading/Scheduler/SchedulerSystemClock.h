#pragma once

#include "GameTimer.h"

#include <mutex>

struct Worker;

class SchedulerSystemClock
{
public:
	SchedulerSystemClock(const int activeThreadCount, std::vector<Worker>* workers, Worker* mainThreadWorker);

	void WaitForSynchronisedLaunch();

	void RegisterActiveThread();
	void UnregisterActiveThread();

	GameTimer* GetClockTimer();
	void CancelFrameSynchronisation();

	void MarkLaunchStartTime();
	void MarkLaunchEndTime();

private:
	void CompleteFrame();
	void SleepUntilNextFrameLaunch();
	float CalculatelargestTimeTakenForWorker() const;

	std::vector<Worker>* workers;
	Worker* mainThreadWorker;

	std::mutex syncMutex;
	std::mutex registrationMutex;

	std::size_t numThreadsToWaitFor;
	std::size_t numActiveThreads;
	std::size_t syncGeneration;
	std::condition_variable launchCondition;

	GameTimer clock;
	float frameTime;
};