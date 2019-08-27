#include "ThreadPackingLayer.h"

#include "ProcessScheduler.h"
#include "SubsystemWorkload.h"
#include "SchedulerSystemClock.h"

const float INSTABILITY_UPDATE_THRESHOLD = 0.5f;

void ThreadPackingLayer::DistributeWorkloadAmongWorkerThreads(SchedulerSystemClock* scheduler, std::vector<Worker>& workers, Worker* mainThreadWorker,
	std::vector<SubsystemWorkload*>& processes, std::vector<SubsystemWorkload*>& mainThreadProcesses)
{
	ClearCurrentWorkload(workers, mainThreadWorker);
	AssignForcedMainThreadWorkToMainThread(mainThreadWorker, mainThreadProcesses);
	SortWorkloadByDescendingSize(processes);
	ScheduleProcessesAmongWorkers(workers, mainThreadWorker, processes);
	PrepareWorkers(scheduler, workers, mainThreadWorker);
}

void ThreadPackingLayer::ClearCurrentWorkload(std::vector<Worker>& workers, Worker* mainThreadWorker)
{
	mainThreadWorker->assignedWorkload.clear();
	mainThreadWorker->currentWorkloadSize = 0.0f;

	for (Worker& worker : workers)
	{
		worker.assignedWorkload.clear();
		worker.currentWorkloadSize = 0.0f;
	}
}

void ThreadPackingLayer::AssignForcedMainThreadWorkToMainThread(Worker* mainThreadWorker,
	std::vector<SubsystemWorkload*>& mainThreadProcesses)
{
	for (SubsystemWorkload* work : mainThreadProcesses)
	{
		mainThreadWorker->assignedWorkload.push_back(work);
		mainThreadWorker->currentWorkloadSize += work->workloadSize;
	}
}

void ThreadPackingLayer::SortWorkloadByDescendingSize(std::vector<SubsystemWorkload*>& processes)
{
	std::sort(processes.begin(), processes.end(), [](SubsystemWorkload* lhs, SubsystemWorkload* rhs)
	{
		return lhs->workloadSize > rhs->workloadSize;
	});
}

void ThreadPackingLayer::ScheduleProcessesAmongWorkers(std::vector<Worker>& workers, Worker* mainThreadWorker,
	std::vector<SubsystemWorkload*>& processes)
{
	for (SubsystemWorkload* work : processes)
	{
		Worker* bestWorker = GetWorkerWithSmallestAllocatedWorkload(workers, mainThreadWorker);
		bestWorker->assignedWorkload.push_back(work);
		bestWorker->currentWorkloadSize += work->workloadSize;
	}
}

Worker* ThreadPackingLayer::GetWorkerWithSmallestAllocatedWorkload(std::vector<Worker>& workers, Worker* mainThreadWorker)
{
	Worker* bestWorker = mainThreadWorker;
	float smallestAllocatedSize = bestWorker->currentWorkloadSize;

	for (Worker& worker : workers)
	{
		if (worker.currentWorkloadSize < smallestAllocatedSize)
		{
			smallestAllocatedSize = worker.currentWorkloadSize;
			bestWorker = &worker;
		}
	}

	return bestWorker;
}

void ThreadPackingLayer::PrepareWorkers(SchedulerSystemClock* scheduler, std::vector<Worker>& workers, Worker* mainThreadWorker)
{
	for (Worker& worker : workers)
	{
		worker.currentWorkloadSize = 0.0f;

		if (!worker.assignedWorkload.empty())
		{
			scheduler->RegisterActiveThread();
		}
	}

	mainThreadWorker->currentWorkloadSize = 0.0f;
	if (!mainThreadWorker->assignedWorkload.empty())
	{
		scheduler->RegisterActiveThread();
	}
}

float ThreadPackingLayer::CalculateTotalSystemInstability(const Worker* workers, const Worker* mainThreadWorker)
{
	float totalSystemInstability = mainThreadWorker->currentWorkerInstability;

	unsigned int numWorkers = ProcessScheduler::Retrieve()->GetTotalNumberOfThreads() - 1;
	for (unsigned int i = 0; i < numWorkers; ++i)
	{
		totalSystemInstability += workers[i].currentWorkerInstability;
	}

	return totalSystemInstability;
}
