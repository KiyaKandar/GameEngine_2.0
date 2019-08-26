#pragma once

#include "Launch/Threading/Scheduler/ProcessScheduler.h"

#include <deque>
#include <mutex>

class Message;

class MessageDeliveryBuffer
{
public:
	MessageDeliveryBuffer()
	{
		unsigned int numThreads = ProcessScheduler::Retrieve()->GetTotalNumberOfThreads();
		buffer = new std::deque<Message*>[numThreads];
		locks = new std::mutex[numThreads];
	}

	~MessageDeliveryBuffer()
	{
		delete[] buffer;
		delete[] locks;
	}

	void Push(Message* message, const unsigned int threadId)
	{
		std::lock_guard<std::mutex> guard(locks[threadId]);
		buffer[threadId].push_back(message);
	}

	Message* Read(const unsigned int threadId, const unsigned int bufferIndex)
	{
		std::lock_guard<std::mutex> guard(locks[threadId]);
		return buffer[threadId][bufferIndex];
	}

	unsigned int Count(const unsigned int threadId)
	{
		std::lock_guard<std::mutex> guard(locks[threadId]);
		return buffer[threadId].size();
	}

	void Clear(const unsigned int numToPopOffFront, unsigned int threadId)
	{
		std::lock_guard<std::mutex> guard(locks[threadId]);
		for (int i = 0; i < numToPopOffFront; ++i)
		{
			buffer[threadId].pop_front();
		}
	}

	void ClearAll()
	{
		unsigned int numThreads = ProcessScheduler::Retrieve()->GetTotalNumberOfThreads();

		for (int threadId = 0; threadId < numThreads; ++threadId)
		{
			for (int i = 0; i < buffer[threadId].size(); ++i)
			{
				if (buffer[threadId][i]->senderAvailable != nullptr)
				{
					*buffer[threadId][i]->senderAvailable = true;
				}
			}

			buffer[threadId].clear();
		}
	}

private:
	std::mutex* locks;
	std::deque<Message*>* buffer;
};
