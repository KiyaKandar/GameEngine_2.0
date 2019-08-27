#pragma once

#include "GameTimer.h"

#include <mutex>
#include "SubsystemWorkload.h"

struct Worker;

class SchedulerSystemClock
{
public:
	SchedulerSystemClock(const int activeThreadCount, std::vector<Worker>* workers, Worker* mainThreadWorker,
		std::vector<SubsystemWorkload*>* processes, std::vector<SubsystemWorkload*>* mainThreadProcesses, Window* window);
	~SchedulerSystemClock();

	void WaitForSynchronisedLaunch();

	void RegisterActiveThread();
	void UnregisterActiveThread();

	GameTimer* GetClockTimer();
	void CancelFrameSynchronisation();

	void MarkLaunchStartTime();
	void MarkLaunchEndTime();

	std::size_t numThreadsToWaitFor;
	std::size_t numActiveThreads;

private:
	void CompleteFrameAsLastFinishedThread();
	void RescheduleWorkloadIfFrameCountDelayElapsed();
	void RelaunchThreadsAtThreadBarrier();

	std::vector<Worker>* workers;
	Worker* mainThreadWorker;

	std::mutex syncMutex;
	std::mutex registrationMutex;

	std::size_t syncGeneration;
	std::condition_variable launchCondition;

	std::vector<SubsystemWorkload*>* processes;
	std::vector<SubsystemWorkload*>* mainThreadProcesses;

	int frameCount = 0;

	GameTimer clock;
	float frameTime;

	Window* window;
};