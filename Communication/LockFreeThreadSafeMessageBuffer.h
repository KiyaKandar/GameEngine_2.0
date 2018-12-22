#pragma once

#include "../Launch/Threading/ThreadPool/ThreadPool.h"
#include "MessageStorage.h"

#include <queue>
#include <utility>

template <class MessageType>
class LockFreeThreadSafeMessageBuffer
{
public:
	LockFreeThreadSafeMessageBuffer()
	{
		numberOfMessageSenders = ThreadPool::getTotalNumberOfThreads();
		outgoingMessages = new std::queue<MessageType>[numberOfMessageSenders];
	}

	~LockFreeThreadSafeMessageBuffer()
	{
		delete[] outgoingMessages;
	}

	void insertOutgoingMessage(MessageType message)
	{
		outgoingMessages[ThreadPool::getLocalThreadId()].push(message);
	}

	void sendMessages(MessageStorage* messageStorage)
	{
		for (unsigned int i = 0; i < numberOfMessageSenders; ++i)
		{
			std::queue<MessageType>& outgoingQueue = outgoingMessages[i];

			while (!outgoingQueue.empty())
			{
				MessageType message = outgoingQueue.front();
				outgoingQueue.pop();

				sentMessages.push(message);
				messageStorage->deliverMessage(&sentMessages.back());
			}
		}
	}

	void clearSentMessages()
	{
		while (!sentMessages.empty())
		{
			sentMessages.pop();
		}
	}

	void clearOutgoingMessages()
	{
		for (unsigned int i = 0; i < numberOfMessageSenders; ++i)
		{
			std::queue<MessageType>& outgoingQueue = outgoingMessages[i];

			while (!outgoingQueue.empty())
			{
				outgoingQueue.pop();
			}
		}
	}

private:
	unsigned int numberOfMessageSenders = 0;
	std::queue<MessageType>* outgoingMessages;
	std::queue<MessageType> sentMessages;
};

