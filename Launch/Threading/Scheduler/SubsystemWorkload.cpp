#include "SubsystemWorkload.h"

//#pragma optimize("", off)

namespace SubsystemThreadIds
{
	thread_local int LOCAL_THREAD_ID = 0;
	unsigned int TOTAL_NUM_THREADS = 0;
}

void Worker::InitialiseTotalNumberOfThreads()
{
	SubsystemThreadIds::TOTAL_NUM_THREADS = std::thread::hardware_concurrency();
}

int Worker::GetLocalThreadId()
{
	return SubsystemThreadIds::LOCAL_THREAD_ID;
}

unsigned Worker::GetTotalNumberOfThreads()
{
	return SubsystemThreadIds::TOTAL_NUM_THREADS;
}

void Worker::SpoolWorker(const unsigned threadId, SchedulerClock* schedulerClock, atomic_int& activeWorkerCount)
{
	if (threadId == 0)
	{
		Run(threadId, activeWorkerCount, schedulerClock);
	}
	else
	{
		workerThread = std::thread(&Worker::Run, this, threadId, std::ref(activeWorkerCount), std::ref(schedulerClock));
	}
}

void Worker::BulkSetWorkload(std::vector<SubsystemWorkload> workload)
{
	assignedWorkload = workload;
}

void Worker::Run(const unsigned int threadId, atomic_int & activeWorkerCount, SchedulerClock* schedulerClock)
{
	SubsystemThreadIds::LOCAL_THREAD_ID = threadId;

	WaitUntilHasWorkload();

	while (SubsystemScheduler::workersRunning)
	{
		timer.BeginTimedSection();

		PerformWork();
		CalculateWorkerInstability();
		//process movementbuffer;

		WaitUntilHasWorkload();

		timer.EndTimedSection();
		//SignalWorkerFinishedIteration(activeWorkerCount, schedulerClock->finishListener);
		WaitForNextSynchronisedLaunch(schedulerClock, activeWorkerCount, timer.GetTimeTakenForSection());
	}
}

void Worker::PerformWork()
{
	currentWorkloadSize = 0.0f;
	for (SubsystemWorkload& work : assignedWorkload)
	{
		work.Tick();
		currentWorkloadSize += work.workloadSize;
	}

	DeliverySystem::GetPostman()->ClearAllMessages();
	DeliverySystem::GetPostman()->DeliverAllMessages();
}

void Worker::WaitUntilHasWorkload()
{
	if (assignedWorkload.empty())
	{
		std::unique_lock<std::mutex> workloadLock(waitForWorkload);
		hasWork.wait(workloadLock, [&assignedWorkload = assignedWorkload]
		{
			return !assignedWorkload.empty() || !SubsystemScheduler::workersRunning;
		});
	}
}

void Worker::WaitForNextSynchronisedLaunch(SchedulerClock* schedulerClock, atomic_int& activeWorkerCount, const float elapsedTimeInMilliseconds)
{
	if (!schedulerClock->TryAssumeControl(activeWorkerCount))
	{
		//wait to be signalled to start
		std::unique_lock<std::mutex> lock(schedulerClock->signalMutex);
		schedulerClock->launcher.wait(lock, [schedulerClock]() { return schedulerClock->launch.load(); });
		activeWorkerCount++;
	}
	else
	{
		activeWorkerCount--;
		schedulerClock->finishListener.notify_one();
	}

	//const float frameLengthMS = (1.0f / 360.0f) * 1000.0f;//16.6666667f;
	//const float timeToSleep = frameLengthMS - elapsedTimeInMilliseconds;

	//if (timeToSleep > 0.0f)
	//{
	//	Sleep(timeToSleep);
	//}
}

void Worker::SignalWorkerFinishedIteration(atomic_int& activeWorkerCount, condition_variable& finishingCondition)
{
	//activeWorkerCount--;
	//finishingCondition.notify_one();
}

void Worker::CalculateWorkerInstability()
{
	currentWorkerInstability = 0.0f;

	for (SubsystemWorkload& work : assignedWorkload)
	{
		currentWorkerInstability += work.CalculateInstability();
	}
}
