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

	void InitialiseWorkers(Window* window) override;

	void RegisterProcess(const Process& process, const std::string debugName) override;
	void AttachMainThreadProcess(const Process& process, const std::string debugName) override;

	void BeginWorkerProcesses() override;
	void CompleteWorkerProcesses() override;

	int GetLocalThreadId() override;
	unsigned int GetTotalNumberOfThreads() override;

	void RegisterWithProfiler(Profiler* profiler) override;

	static std::atomic_bool workersRunning;

private:
	void StopAllNonMainThreadWorkers();
	void ClearWorkersFromScheduler();
	void DeleteAllRegisteredProcesses();

	std::vector<SubsystemWorkload*> registeredProcesses;
	std::vector<SubsystemWorkload*> registeredProcessesLockedToMainThread;

	std::vector<Worker> workers;
	Worker* mainThreadWorker;

	std::atomic_bool running;

	SchedulerSystemClock* schedulerClock;
};
