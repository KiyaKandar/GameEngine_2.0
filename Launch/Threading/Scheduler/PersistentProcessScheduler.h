#pragma once

#include "ProcessScheduler.h"

#include "../ThreadUtility/ThreadQueue.h"
#include "../ThreadUtility/Task.h"
#include "../ThreadUtility/ThreadTask.h"
#include "../ThreadUtility/TaskFuture.h"

#include <functional>
#include <queue>
#include <thread>
#include <future>

typedef TaskFuture<void> ProcessPromise;

class PersistentProcessScheduler : public ProcessScheduler
{
public:
	PersistentProcessScheduler();
	~PersistentProcessScheduler() = default;

	void InitialiseWorkers() override {}

	void RegisterProcess(const Process& process) override;
	void AttachMainThreadProcess(const Process& process) override;

	void ExecuteMainThreadTask() override;
	void BeginWorkerProcesses() override;
	void CompleteWorkerProcesses() override;

	int GetLocalThreadId() override;
	unsigned int GetTotalNumberOfThreads() override;

	void RegisterWithProfiler(Profiler* profiler) override {}

private:
	void WorkerThreadProcess(const int threadId);

	ThreadQueue<std::unique_ptr<Task>> taskQueue;
	std::vector<Process> registeredProcesses;
	std::vector<ProcessPromise> promises;
	std::vector<std::thread> availableWorkerThreads;

	Process attachedMainThreadTask;

	std::atomic_bool running;
	std::atomic_bool completeWorkerProcesses;
};
