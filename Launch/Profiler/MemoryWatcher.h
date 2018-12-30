#pragma once

#include <string.h>
#include <iostream>

class Database;

class MemoryWatcher
{
public:
	MemoryWatcher() = default;
	MemoryWatcher(size_t max, Database* database);
	~MemoryWatcher();

	void Update();

	float percent;
	float bytesleft;

protected:
	Database* database;
	size_t maxUsage;
	size_t currentUsage;
};
