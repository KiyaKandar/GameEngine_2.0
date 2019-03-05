#pragma once

struct Worker;

class ThreadPackingLayer
{
public:
	static void DistributeWorkloadAmongWorkerThreads(Worker* workers, Worker* mainThreadWorker);

private:
	static float CalculateTotalSystemInstability(const Worker* workers, const Worker* mainThreadWorker);
};

