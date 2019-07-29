#include "PersistentProcessScheduler.h"

typedef std::packaged_task<void()> ASyncReadyProcess;
typedef ThreadTask<ASyncReadyProcess> ExecutableProcess;

namespace PersistentThreadIds
{
	thread_local int LOCAL_THREAD_ID = 0;
	unsigned int TOTAL_NUM_THREADS = 0;
}

PersistentProcessScheduler::PersistentProcessScheduler() : ProcessScheduler()
{
	PersistentThreadIds::TOTAL_NUM_THREADS = std::thread::hardware_concurrency();
	running = true;
	completeWorkerProcesses = false;
}

void PersistentProcessScheduler::RegisterProcess(const Process& process, const std::string debugName)
{
	ASyncReadyProcess task{ std::move(process) };
	promises.push_back(ProcessPromise{ task.get_future() });
	taskQueue.Push(std::make_unique<ExecutableProcess>(std::move(task)));
}

void PersistentProcessScheduler::AttachMainThreadProcess(const Process& process, const std::string debugName)
{
	attachedMainThreadTask = process;
}

void PersistentProcessScheduler::BeginWorkerProcesses()
{
	for (int i = 0; i < PersistentThreadIds::TOTAL_NUM_THREADS - 1; ++i)
	{
		const int threadId = i + 1;
		availableWorkerThreads.emplace_back(&PersistentProcessScheduler::WorkerThreadProcess, this, threadId);
	}

	attachedMainThreadTask();
}

void PersistentProcessScheduler::CompleteWorkerProcesses()
{
	completeWorkerProcesses = true;

	for (ProcessPromise& future : promises)
	{
		future.Complete();
	}

	promises.clear();
}

int PersistentProcessScheduler::GetLocalThreadId()
{
	return PersistentThreadIds::LOCAL_THREAD_ID;
}

unsigned PersistentProcessScheduler::GetTotalNumberOfThreads()
{
	return PersistentThreadIds::TOTAL_NUM_THREADS;
}

void PersistentProcessScheduler::WorkerThreadProcess(const int threadId)
{
	PersistentThreadIds::LOCAL_THREAD_ID = threadId;

	while (running)
	{
		std::unique_ptr<Task> newTask = nullptr;

		if (taskQueue.GetAvailableTask(newTask))
		{
			newTask->Execute();
		}
	}
}
