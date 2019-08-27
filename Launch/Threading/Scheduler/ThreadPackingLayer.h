#pragma once
#include <vector>
#include "SubsystemWorkload.h"

struct Worker;
class SchedulerSystemClock;

class ThreadPackingLayer
{
public:
	static void DistributeWorkloadAmongWorkerThreads(SchedulerSystemClock* scheduler, std::vector<Worker>& workers, Worker* mainThreadWorker,
		std::vector<SubsystemWorkload*>& processes, std::vector<SubsystemWorkload*>& mainThreadProcesses);

private:
	static void ClearCurrentWorkload(std::vector<Worker>& workers, Worker* mainThreadWorker);
	static void AssignForcedMainThreadWorkToMainThread(Worker* mainThreadWorker, std::vector<SubsystemWorkload*>& mainThreadProcesses);
	static void SortWorkloadByDescendingSize(std::vector<SubsystemWorkload*>& processes);
	static void ScheduleProcessesAmongWorkers(std::vector<Worker>& workers, Worker* mainThreadWorker,
		std::vector<SubsystemWorkload*>& processes);
	static Worker* GetWorkerWithSmallestAllocatedWorkload(std::vector<Worker>& workers, Worker* mainThreadWorker);
	static void PrepareWorkers(SchedulerSystemClock* scheduler, std::vector<Worker>& workers, Worker* mainThreadWorker);

	static float CalculateTotalSystemInstability(const Worker* workers, const Worker* mainThreadWorker);
};

