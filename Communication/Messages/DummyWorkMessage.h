#pragma once

#include "../Message.h"

struct Node;

class DummyWorkMessage : public Message
{
public:
	DummyWorkMessage() {}
	DummyWorkMessage(const std::string& destination, const int workloadDurationMicroseconds, bool randomiseDestination);
	~DummyWorkMessage();

	void RandomiseDestination();
	void DoDummyWork() const;

	static DummyWorkMessage Builder(Node* node);

private:
	void ForceMemoryAllocationsAndCacheMisses() const;

	int workloadDurationMicroseconds = 0.0f;
	bool randomiseDestination = true;
};

