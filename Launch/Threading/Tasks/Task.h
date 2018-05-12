#pragma once

class Task
{
public:
	Task() = default;
	Task(Task&& other) = default;

	virtual ~Task() = default;

	virtual void execute() = 0;

	Task& operator=(Task&& other) = default;
};

