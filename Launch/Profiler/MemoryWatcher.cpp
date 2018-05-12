#include "MemoryWatcher.h"

#include "../Resource Management/Database/Database.h"

MemoryWatcher::MemoryWatcher(size_t max, Database* database)
{
	this->maxUsage = max;
	this->database = database;
}

MemoryWatcher::~MemoryWatcher()
{
}

void MemoryWatcher::Update()
{
	currentUsage = database->CurrentSize();

	percent = (static_cast<float>(currentUsage) / static_cast<float>(maxUsage)) * 100;
	bytesleft = static_cast<float>(maxUsage) - static_cast<float>(currentUsage);
}