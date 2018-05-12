#pragma once

#include "Task.h"

#include <utility>

template <typename Function>
class ThreadTask : public Task
{
public:
	ThreadTask(Function&& func)
	{
		function = move(func); //Speed things up a bit...
	}

	ThreadTask(ThreadTask&& other)	= default;

	~ThreadTask() override = default;

	void execute() override 
	{
		function();
	}

	ThreadTask& operator=(ThreadTask&& other) = default;
private:
	Function function;
};

