#include "ThreadPackingLayer.h"

#include "ProcessScheduler.h"
#include "SubsystemWorkload.h"

const float INSTABILITY_UPDATE_THRESHOLD = 0.5f;

void ThreadPackingLayer::DistributeWorkloadAmongWorkerThreads(std::vector<Worker>& workers, Worker* mainThreadWorker,
	std::vector<SubsystemWorkload*> processes, std::vector<SubsystemWorkload*> mainThreadProcesses)
{
	mainThreadWorker->assignedWorkload.clear();
	mainThreadWorker->currentWorkloadSize = 0.0f;

	for (Worker& worker : workers)
	{
		worker.assignedWorkload.clear();
		worker.currentWorkloadSize = 0.0f;
	}

	for (SubsystemWorkload* work : mainThreadProcesses)
	{
		mainThreadWorker->assignedWorkload.push_back(work);
		mainThreadWorker->currentWorkloadSize += work->workloadSize;
	}

	std::sort(processes.begin(), processes.end(), [](SubsystemWorkload* lhs, SubsystemWorkload* rhs)
	{
		return lhs->workloadSize > rhs->workloadSize;
	});

	for (SubsystemWorkload* work : processes)
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

		bestWorker->assignedWorkload.push_back(work);
		bestWorker->currentWorkloadSize += work->workloadSize;
	}


	for (Worker& worker : workers)
	{
		worker.currentWorkloadSize = 0.0f;
	}
	mainThreadWorker->currentWorkloadSize = 0.0f;
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
