#include "ThreadPool.h"

thread_local int LOCAL_THREAD_ID = 0;
extern unsigned int TOTAL_NUM_THREADS = 0;

ThreadPool::ThreadPool(const int numThreads)
{
	running = true;
	InitialiseWorkers(numThreads);
}

ThreadPool::ThreadPool()
{
	running = true;

	TOTAL_NUM_THREADS = std::thread::hardware_concurrency();
	const int numThreads = TOTAL_NUM_THREADS - 1;

	InitialiseWorkers(numThreads);
}

int ThreadPool::GetLocalThreadId()
{
	return LOCAL_THREAD_ID;
}

unsigned int ThreadPool::GetTotalNumberOfThreads()
{
	return TOTAL_NUM_THREADS;
}

void ThreadPool::InitialiseWorkers(int numWorkers)
{
	for (int i = 0; i < numWorkers; ++i)
	{
		const int threadId = i + 1;
		threads.emplace_back(&ThreadPool::SpoolThreadToPollNewTasks, this, threadId);
	}
}

void ThreadPool::SpoolThreadToPollNewTasks(const int threadId)
{
	LOCAL_THREAD_ID = threadId;

	while (running)
	{
		std::unique_ptr<Task> newTask = nullptr;

		if (taskQueue.GetAvailableTask(newTask))
		{
			newTask->Execute();
		}
	}
}

void ThreadPool::JoinAllThreads()
{
	for (auto& thread : threads)
	{
		if (thread.joinable())
		{
			thread.join(); //Stop all threads.
		}
	}
}
