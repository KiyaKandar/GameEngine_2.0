#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

/*
  Wrapper around a queue.
  Allows a series of tasks to be performed.
  Will always be threadsafe.
*/
template <typename T>
class ThreadQueue
{
public:
	~ThreadQueue()
	{
		invalidate();
	}

	bool getAvailableTask(T& out)
	{
		std::unique_lock<std::mutex> lock(queueLock);

		condition.wait(lock, [this]()
		{
			return !tasks.empty() || !valid;
		});

		if (!valid)
		{
			return false;
		}

		out = move(tasks.front());
		tasks.pop();

		return true;
	}

	void push(T value) 
	{
		std::unique_lock<std::mutex> lock(queueLock);

		tasks.push(move(value));
		condition.notify_one();
	}

	bool isEmpty() const
	{
		std::lock_guard<std::mutex> lock(queueLock);

		return tasks.empty();
	}

	void deleteAllTasks()
	{
		std::lock_guard<std::mutex> lock(queueLock);

		while (!tasks.empty()) 
		{
			tasks.pop();
		}

		condition.notify_all();
	}

	void invalidate()
	{
		std::lock_guard<std::mutex> lock(queueLock);

		valid = false;
		condition.notify_all();
	}

		
	bool isValid() const
	{
		std::lock_guard<std::mutex> lock(queueLock);

		return valid;
	}

private:
	std::atomic_bool valid { true };

	mutable std::mutex queueLock;
	std::queue<T> tasks;
	std::condition_variable condition;
};