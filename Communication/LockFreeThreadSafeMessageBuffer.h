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
		sentMessages = new std::queue<MessageType>[numberOfMessageSenders];
	}

	~LockFreeThreadSafeMessageBuffer()
	{
		delete[] outgoingMessages;
		delete[] sentMessages;
	}

	void insertOutgoingMessage(MessageType message)
	{
		outgoingMessages[ThreadPool::getLocalThreadId()].push(message);
	}

	void sendMessages(MessageStorage* messageStorage)
	{
		unsigned int localThreadId = ThreadPool::getLocalThreadId();

		std::queue<MessageType>& outgoingQueue = outgoingMessages[localThreadId];

		while (!outgoingQueue.empty())
		{
			MessageType message = outgoingQueue.front();
			outgoingQueue.pop();

			sentMessages[localThreadId].push(message);
			messageStorage->deliverMessage(&sentMessages[localThreadId].back(), localThreadId);
		}
	}

	void clearSentMessages()
	{
		unsigned int localThreadId = ThreadPool::getLocalThreadId();

		while (!sentMessages[localThreadId].empty() && sentMessages[localThreadId].front().processed)
		{
			sentMessages[localThreadId].pop();
		}
	}

	void cancelOutgoingMessages()
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
	std::queue<MessageType>* sentMessages;
};

