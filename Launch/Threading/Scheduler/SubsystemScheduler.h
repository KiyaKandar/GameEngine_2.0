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

struct SubsystemWorkload;
struct Worker;
struct SchedulerSystemClock;

class SubsystemScheduler : public ProcessScheduler
{
public:
	SubsystemScheduler();
	~SubsystemScheduler();

	void InitialiseWorkers() override;

	void RegisterProcess(const Process& process, const std::string debugName) override;
	void AttachMainThreadProcess(const Process& process, const std::string debugName) override;

	void ExecuteMainThreadTask() override;
	void BeginWorkerProcesses() override;
	void CompleteWorkerProcesses() override;

	int GetLocalThreadId() override;
	unsigned int GetTotalNumberOfThreads() override;

	void RegisterWithProfiler(Profiler* profiler) override;

	static std::atomic_bool workersRunning;

private:
	std::vector<SubsystemWorkload> swl;
	std::vector<Worker> workers;
	Worker* mainThreadWorker;

	std::atomic_bool running;

	SchedulerSystemClock* schedulerClock;
};
