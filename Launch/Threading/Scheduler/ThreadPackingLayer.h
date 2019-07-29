#pragma once
#include <vector>
#include "SubsystemWorkload.h"

struct Worker;

class ThreadPackingLayer
{
public:
	static void DistributeWorkloadAmongWorkerThreads(std::vector<Worker>& workers, Worker* mainThreadWorker, 
		std::vector<SubsystemWorkload*> processes, std::vector<SubsystemWorkload*> mainThreadProcesses);

private:
	static float CalculateTotalSystemInstability(const Worker* workers, const Worker* mainThreadWorker);
};

