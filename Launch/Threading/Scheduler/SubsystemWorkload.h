#pragma once

#include "SubsystemScheduler.h"
#include "GameTimer.h"
#include "Communication/DeliverySystem.h"

const unsigned int NO_WORKER_MOVE = -1;

struct SchedulerSystemClock;

struct SubsystemWorkload
{
	SubsystemWorkload(Process process, const std::string& debugName)
	{
		this->process = process;
		this->debugName = debugName;
	}

	void Tick()
	{
		timer.BeginTimedSection();
		process();
		timer.EndTimedSection();

		workloadSize = timer.GetTimeTakenForSection();
	}

	float CalculateInstability()
	{
		return 0.0f;
	}

	float instability = 0.0f;
	float workloadSize = 0.0f;
	bool lockedToMainThread = false;
	unsigned int moveToWorker = NO_WORKER_MOVE;

	std::string debugName;

private:
	Process process;
	GameTimer timer;
};

struct Worker
{
	void SetSchedulerClock(SchedulerSystemClock* schedulerClock);
	static void InitialiseTotalNumberOfThreads();
	static int GetLocalThreadId();
	static unsigned int GetTotalNumberOfThreads();

	std::vector<SubsystemWorkload*> assignedWorkload; //use a threadsafe queue
	float maximumWorkloadSize = 1.0 / 60.0f; //TODO - Dynamically calculate this
	float currentWorkloadSize = 0.0f;
	float currentWorkerInstability = 0.0f;
	std::thread workerThread;

	std::mutex waitForWorkload;
	std::condition_variable hasWork;
	std::condition_variable waitForNextLaunch;

	void SpoolWorker(const unsigned int threadId);

	void Run(const unsigned int threadId);
	void PerformWork();

	void WaitUntilHasActivity();
	void WaitUntilHasWorkload();

	void CalculateWorkerInstability();

	void ClearWorkload()
	{
		assignedWorkload.clear();
	}

	SchedulerSystemClock* schedulerClock = nullptr;
	//movementbuffer buf;
};
