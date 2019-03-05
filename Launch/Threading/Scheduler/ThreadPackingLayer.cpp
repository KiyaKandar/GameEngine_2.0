#include "ThreadPackingLayer.h"

#include "ProcessScheduler.h"
#include "SubsystemWorkload.h"

const float INSTABILITY_UPDATE_THRESHOLD = 0.5f;

void ThreadPackingLayer::DistributeWorkloadAmongWorkerThreads(Worker* workers, Worker* mainThreadWorker)
{
	unsigned int numWorkers = ProcessScheduler::Retrieve()->GetTotalNumberOfThreads() - 1;
	std::vector<Worker*> workersExceedingMaxWorkload(numWorkers);
	unsigned int numWorkersExceedingMaxWorkload = 0;

	for (unsigned int i = 0; i < numWorkers; ++i)
	{
		if (workers[i].currentWorkloadSize > workers[i].maximumWorkloadSize)
		{
			workersExceedingMaxWorkload[numWorkersExceedingMaxWorkload] = &workers[i];
			numWorkersExceedingMaxWorkload++;
		}
	}

	int indexOfWorkerWithMostExcess = -1;
	for (unsigned int i = 0; i < numWorkersExceedingMaxWorkload; ++i)
	{
		
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
