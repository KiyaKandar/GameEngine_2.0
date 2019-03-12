#pragma once

#include <functional>
#include "../../Profiler/Profiler.h"
#include "../ThreadUtility/TaskFuture.h"

typedef std::function<void()> Process;

class ProcessScheduler
{
public:
	static ProcessScheduler* Retrieve()
	{
		return scheduler;
	}

	static void Create(ProcessScheduler* newScheduler)
	{
		scheduler = newScheduler;
	}

	virtual void InitialiseWorkers() = 0;

	virtual void RegisterProcess(const Process& process) = 0;
	virtual void AttachMainThreadProcess(const Process& process) = 0;

	virtual void ExecuteMainThreadTask() = 0;
	virtual void BeginWorkerProcesses() = 0;
	virtual void CompleteWorkerProcesses() = 0;

	virtual int GetLocalThreadId() = 0;
	virtual unsigned int GetTotalNumberOfThreads() = 0;

	virtual void RegisterWithProfiler(Profiler* profiler) = 0;

protected:
	ProcessScheduler() {}
	virtual ~ProcessScheduler() {}

private:
	static ProcessScheduler* scheduler;
};