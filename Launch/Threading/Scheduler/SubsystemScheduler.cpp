#include "SubsystemScheduler.h"

#include "SubsystemWorkload.h"

atomic_bool SubsystemScheduler::workersRunning = false;

SubsystemScheduler::SubsystemScheduler() : ProcessScheduler()
{
	Worker::InitialiseTotalNumberOfThreads();
	running = true;
	workersRunning = true;

	schedulerClock = new SchedulerClock();
}

SubsystemScheduler::~SubsystemScheduler()
{
	delete[] workers;
	delete mainThreadWorker;
}

void SubsystemScheduler::InitialiseWorkers()
{
	workers = new Worker[Worker::GetTotalNumberOfThreads() - 1];
	mainThreadWorker = new Worker();
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
	mainThreadWorker->SpoolWorker(0, schedulerClock, activeWorkerCount);
}

void SubsystemScheduler::BeginWorkerProcesses()
{
	workers[0].BulkSetWorkload(swl);
	activeWorkerCount.store(1);
	workersRunning = true;

	unsigned int numThreads = Worker::GetTotalNumberOfThreads();

	for (int i = 0; i < numThreads - 1; ++i)
	{
		const int threadId = i + 1;
		workers[i].SpoolWorker(threadId, schedulerClock, activeWorkerCount);
	}
}

void SubsystemScheduler::CompleteWorkerProcesses()
{
	workersRunning = false;
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
