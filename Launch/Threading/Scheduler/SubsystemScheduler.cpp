#include "SubsystemScheduler.h"

#include "SubsystemWorkload.h"
#include "SchedulerSystemClock.h"

atomic_bool SubsystemScheduler::workersRunning = false;

SubsystemScheduler::SubsystemScheduler() : ProcessScheduler()
{
	Worker::InitialiseTotalNumberOfThreads();
	running = true;
	workersRunning = true;

	schedulerClock = new SchedulerSystemClock(1);
}

SubsystemScheduler::~SubsystemScheduler()
{
	delete mainThreadWorker;
}

void SubsystemScheduler::InitialiseWorkers()
{
	const unsigned int numWorkers = Worker::GetTotalNumberOfThreads() - 1;
	workers = std::vector<Worker>(numWorkers);

	for (int i = 0; i < numWorkers; ++i)
	{
		workers[i].SetSchedulerClock(schedulerClock);
	}

	mainThreadWorker = new Worker();
	mainThreadWorker->SetSchedulerClock(schedulerClock);
}

void SubsystemScheduler::RegisterProcess(const Process& process)
{
	swl.push_back(SubsystemWorkload(process));
}

void SubsystemScheduler::AttachMainThreadProcess(const Process& process)
{
	mainThreadWorker->assignedWorkload.push_back(process);
}

void SubsystemScheduler::ExecuteMainThreadTask()
{
	mainThreadWorker->SpoolWorker(0);
}

void SubsystemScheduler::BeginWorkerProcesses()
{
	int workerId = 0;
	unsigned int numThreads = Worker::GetTotalNumberOfThreads();

	while (!swl.empty())
	{
		workers[workerId].assignedWorkload.push_back(swl.back());
		swl.pop_back();

		++workerId;
		if (workerId == numThreads - 1)
		{
			workerId = 0;
		}
	}

	workersRunning = true;

	schedulerClock->MarkLaunchStartTime();
	for (int i = 0; i < numThreads - 1; ++i)
	{
		const int threadId = i + 1;
		workers[i].SpoolWorker(threadId);
	}
}

void SubsystemScheduler::CompleteWorkerProcesses()
{
	workersRunning = false;
	schedulerClock->CancelFrameSynchronisation();

	bool finished = false;

	unsigned int numThreads = Worker::GetTotalNumberOfThreads();
	for (int i = 0; i < numThreads - 1; ++i)
	{
		workers[i].hasWork.notify_one();
		workers[i].workerThread.join();
	}

	swl.clear();

	for (int i = 0; i < numThreads - 1; ++i)
	{
		workers[i].ClearWorkload();
		schedulerClock->UnregisterActiveThread();
	}
}

int SubsystemScheduler::GetLocalThreadId()
{
	return Worker::GetLocalThreadId();
}

unsigned SubsystemScheduler::GetTotalNumberOfThreads()
{
	return Worker::GetTotalNumberOfThreads();
}
