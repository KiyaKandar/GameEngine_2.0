#pragma once
#include <future>

/*
  Wrapper around std::future.
  It will block and wait for execution to finish before calling destructor.
*/
template <typename T>
class TaskFuture
{
public:
	TaskFuture(std::future<T>&& future)
	{
		tFuture = move(future);
	}

	TaskFuture(TaskFuture&& other) = default;

	~TaskFuture() 
	{
		//if (tFuture.valid())
		//{
		//	tFuture.get();
		//}
	}

	auto Complete()
	{
		//if (tFuture.valid())
		//{
			return tFuture.get();
		//}
	}

	TaskFuture& operator=(TaskFuture&& other) = default;
private:
	std::future<T> tFuture;
};