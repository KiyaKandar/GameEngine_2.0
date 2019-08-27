#include "SubsystemScheduler.h"

#include "SubsystemWorkload.h"
#include "SchedulerSystemClock.h"
#include "ThreadPackingLayer.h"

atomic_bool SubsystemScheduler::workersRunning = false;

SubsystemScheduler::SubsystemScheduler() : ProcessScheduler()
{
	Worker::InitialiseTotalNumberOfThreads();
	running = true;
	workersRunning = true;
}

SubsystemScheduler::~SubsystemScheduler()
{
	delete mainThreadWorker;
}

void SubsystemScheduler::InitialiseWorkers(Window* window)
{
	const unsigned int numWorkers = Worker::GetTotalNumberOfThreads() - 1;
	workers = std::vector<Worker>(numWorkers);
	mainThreadWorker = new Worker();

	schedulerClock = new SchedulerSystemClock(0, &workers, mainThreadWorker, 
		&registeredProcesses, &registeredProcessesLockedToMainThread, window);

	for (int i = 0; i < numWorkers; ++i)
	{
		workers[i].SetSchedulerClock(schedulerClock);
	}

	mainThreadWorker->SetSchedulerClock(schedulerClock);
}

void SubsystemScheduler::RegisterProcess(const Process& process, const std::string debugName)
{
	registeredProcesses.push_back(new SubsystemWorkload(process, debugName));
}

void SubsystemScheduler::AttachMainThreadProcess(const Process& process, const std::string debugName)
{
	registeredProcessesLockedToMainThread.push_back(new SubsystemWorkload(process, debugName));
}

void SubsystemScheduler::BeginWorkerProcesses()
{
	unsigned int numThreads = Worker::GetTotalNumberOfThreads();

	schedulerClock->numThreadsToWaitFor = 0;
	schedulerClock->numActiveThreads = 0;
	ThreadPackingLayer::DistributeWorkloadAmongWorkerThreads(schedulerClock, workers, mainThreadWorker,
		registeredProcesses, registeredProcessesLockedToMainThread);

	schedulerClock->numActiveThreads = schedulerClock->numThreadsToWaitFor;

	workersRunning = true;
	schedulerClock->MarkLaunchStartTime();

	for (int i = 0; i < numThreads - 1; ++i)
	{
		const int threadId = i + 1;
		workers[i].SpoolWorker(threadId);
	}

	mainThreadWorker->SpoolWorker(0);
}

void SubsystemScheduler::CompleteWorkerProcesses()
{
	workersRunning = false;

	StopAllNonMainThreadWorkers(); 
	ClearWorkersFromScheduler();
	DeleteAllRegisteredProcesses();
}

int SubsystemScheduler::GetLocalThreadId()
{
	return Worker::GetLocalThreadId();
}

unsigned SubsystemScheduler::GetTotalNumberOfThreads()
{
	return Worker::GetTotalNumberOfThreads();
}

void SubsystemScheduler::RegisterWithProfiler(Profiler* profiler)
{
	profiler->AddSubsystemTimer("System Frame", schedulerClock->GetClockTimer());
	profiler->RegisterWorkers(&workers, mainThreadWorker);
}

void SubsystemScheduler::StopAllNonMainThreadWorkers()
{
	schedulerClock->CancelFrameSynchronisation();

	const unsigned int numThreads = Worker::GetTotalNumberOfThreads();
	for (int i = 0; i < numThreads - 1; ++i)
	{
		workers[i].hasWork.notify_one();
		workers[i].workerThread.join();
	}
}

void SubsystemScheduler::ClearWorkersFromScheduler()
{
	const unsigned int numThreads = Worker::GetTotalNumberOfThreads();
	for (int i = 0; i < numThreads - 1; ++i)
	{
		workers[i].ClearWorkload();
		schedulerClock->UnregisterActiveThread();
	}
}

void SubsystemScheduler::DeleteAllRegisteredProcesses()
{
	for (int i = 0; i < registeredProcesses.size(); ++i)
	{
		delete registeredProcesses[i];
	}

	for (int i = 0; i < registeredProcessesLockedToMainThread.size(); ++i)
	{
		delete registeredProcessesLockedToMainThread[i];
	}

	registeredProcesses.clear();
	registeredProcessesLockedToMainThread.clear();
}
