#pragma once

#include "SubsystemScheduler.h"
#include "GameTimer.h"
#include "Communication/DeliverySystem.h"

const unsigned int NO_WORKER_MOVE = -1;

struct SubsystemWorkload
{
	SubsystemWorkload(Process process)
	{
		this->process = process;
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

private:
	Process process;
	GameTimer timer;
};

class InterruptableSleeper {
	std::timed_mutex mut_;
	std::atomic_bool locked_;

	void lock() 
	{ 
		mut_.lock();
		locked_ = true;
	}

	void unlock() 
	{
		locked_ = false;
		mut_.unlock();
	}
public:
	~InterruptableSleeper() {
		if (locked_) {
			unlock();
		}
	}

	void sleepFor(const float m) 
	{
		if (mut_.try_lock_for(std::chrono::duration<float, milli>(m)))
		{
			mut_.unlock();
		}
	}

	void wake() {
		if (locked_)
		{
			unlock();
		}
	}
};

struct SchedulerClock
{
	bool TryAssumeControl(atomic_int& activeThreadCounter)
	{
		if (clockControlMutex.try_lock()) 
		{
			activeThreadCounter--;
			launch.store(false);
			//const int numActiveThreads = activeThreadCounter.load();

			//Wait for other threads to finish 
			std::unique_lock<std::mutex> lock(finishingLock);
			finishListener.wait(lock, [&activeThreadCounter]() { return activeThreadCounter <= 0; /*OR 60fps IS DONE*/});

			//Do scheduling things with all threads synced up
			//TODO

			//then signal start/launch
			launch.store(true);
			launcher.notify_all();

			clockControlMutex.unlock();
			return true;
		}

		return false;
	}

	std::condition_variable launcher;
	std::condition_variable finishListener;

	std::mutex signalMutex;
	std::mutex clockControlMutex;

	atomic_bool launch;

private:
	std::mutex finishingLock;
};

struct Worker
{
	static void InitialiseTotalNumberOfThreads();
	static int GetLocalThreadId();
	static unsigned int GetTotalNumberOfThreads();

	std::vector<SubsystemWorkload> assignedWorkload; //use a threadsafe queue
	float maximumWorkloadSize = 1.0 / 60.0f; //TODO - Dynamically calculate this
	float currentWorkloadSize = 0.0f;
	float currentWorkerInstability = 0.0f;
	std::thread workerThread;

	std::mutex waitForWorkload;
	std::condition_variable hasWork;
	std::condition_variable waitForNextLaunch;

	void SpoolWorker(const unsigned int threadId, SchedulerClock* schedulerClock, atomic_int& activeWorkerCount);
	void BulkSetWorkload(std::vector<SubsystemWorkload> workload);

	void Run(const unsigned int threadId, atomic_int& activeWorkerCount, SchedulerClock* schedulerClock);
	void PerformWork();

	void WaitUntilHasWorkload();
	void WaitForNextSynchronisedLaunch(SchedulerClock* schedulerClock, atomic_int& activeWorkerCount, const float elapsedTimeInMilliseconds);
	void SignalWorkerFinishedIteration(atomic_int& activeWorkerCount, condition_variable& finishingCondition);

	void CalculateWorkerInstability();

	void ClearWorkload()
	{
		assignedWorkload.clear();
	}

	InterruptableSleeper sleeper;
	GameTimer timer;
	//movementbuffer buf;
};
