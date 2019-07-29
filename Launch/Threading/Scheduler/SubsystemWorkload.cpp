#include "SubsystemWorkload.h"

#include "SchedulerSystemClock.h"

namespace SubsystemThreadIds
{
	thread_local int LOCAL_THREAD_ID = 0;
	unsigned int TOTAL_NUM_THREADS = 0;
}

void Worker::SetSchedulerClock(SchedulerSystemClock* schedulerClock)
{
	this->schedulerClock = schedulerClock;
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

void Worker::SpoolWorker(const unsigned threadId)
{
	if (threadId == 0)
	{
		Run(threadId);
	}
	else
	{
		workerThread = std::thread(&Worker::Run, this, threadId);
	}
}

void Worker::BulkSetWorkload(std::vector<SubsystemWorkload> workload)
{
	//assignedWorkload = workload;
}

void Worker::Run(const unsigned int threadId)
{
	SubsystemThreadIds::LOCAL_THREAD_ID = threadId;

	if (assignedWorkload.empty())
	{
		WaitUntilHasWorkload();
	}
	else if (threadId != 0)
	{
		schedulerClock->RegisterActiveThread();
	}

	while (SubsystemScheduler::workersRunning)
	{
		PerformWork();
		CalculateWorkerInstability();
		//process movementbuffer;

		WaitUntilHasActivity();
	}
}

void Worker::PerformWork()
{
	float frameWorkload = 0.0f;
	for (SubsystemWorkload* work : assignedWorkload)
	{
		work->Tick();
		frameWorkload += work->workloadSize;
	}

	currentWorkloadSize = frameWorkload;

	DeliverySystem::GetPostman()->ClearAllMessages();
	DeliverySystem::GetPostman()->DeliverAllMessages();
}

void Worker::WaitUntilHasActivity()
{
	if (assignedWorkload.empty())
	{
		currentWorkloadSize = 0.0f;
		schedulerClock->UnregisterActiveThread();
		WaitUntilHasWorkload();
	}
	else
	{
		schedulerClock->WaitForSynchronisedLaunch();
	}
}

void Worker::WaitUntilHasWorkload()
{
	std::unique_lock<std::mutex> workloadLock(waitForWorkload);
	std::cout << "Thread " << SubsystemThreadIds::LOCAL_THREAD_ID << " waiting for work." << std::endl;
	hasWork.wait(workloadLock, [&assignedWorkload = assignedWorkload]
	{
		return !assignedWorkload.empty() || !SubsystemScheduler::workersRunning;
	});

	schedulerClock->RegisterActiveThread();
}

void Worker::CalculateWorkerInstability()
{
	currentWorkerInstability = 0.0f;

	for (SubsystemWorkload* work : assignedWorkload)
	{
		currentWorkerInstability += work->CalculateInstability();
	}
}
