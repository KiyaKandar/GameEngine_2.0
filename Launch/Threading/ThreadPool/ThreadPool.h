#pragma once

#include "ThreadQueue.h"
#include "../Tasks/ThreadTask.h"
#include "../Tasks/TaskFuture.h"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

/*
  Bunch of threads ready to carry tasks out.
*/
class ThreadPool
{
public:
	ThreadPool();
	ThreadPool(const int numThreads);

	~ThreadPool() 
	{
		running = false;
		taskQueue.invalidate();

		joinAllThreads();
	}

	template <typename Function, typename... Params>
	inline auto submitJob(Function&& func, Params&&... params)
	{
		auto thisTask = bind(forward<Function>(func), forward<Params>(params)...);

		using resultType = std::result_of_t<decltype(thisTask)()>; //What will the function return?
		using packagedTask = std::packaged_task<resultType()>;		 //Prepare the function for async...
		using taskType = ThreadTask<packagedTask>;			 //Create a thread task for that function.

		packagedTask task{ move(thisTask) };
		TaskFuture<resultType> result{ task.get_future() };

		taskQueue.push(std::make_unique<taskType>(move(task)));
		return result;
	}

private:
	void initialiseWorkers(int numWorkers);
	void continiouslyPollForNewTask();
	void joinAllThreads();

	std::atomic_bool running;
	ThreadQueue<std::unique_ptr<Task>> taskQueue;
	std::vector<std::thread> threads;
};
