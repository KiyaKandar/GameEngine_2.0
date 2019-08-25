#pragma once

#include "MessageStorage.h"
#include "LockFreeQueue.h"

#include <queue>
#include <utility>
#include "Launch/Threading/Scheduler/ProcessScheduler.h"

template <class MessageType>
class LockFreeThreadSafeMessageBuffer
{
public:
	LockFreeThreadSafeMessageBuffer()
	{
		numberOfMessageSenders = ProcessScheduler::Retrieve()->GetTotalNumberOfThreads();
		outgoingMessages = new std::queue<MessageType>[numberOfMessageSenders];
	}

	~LockFreeThreadSafeMessageBuffer()
	{
		delete[] outgoingMessages;
	}

	void InsertOutgoingMessage(MessageType message, MessageStorage* messageStorage)
	{
		unsigned int localThreadId = ProcessScheduler::Retrieve()->GetLocalThreadId();
		outgoingMessages[localThreadId].push(message);
		messageStorage->DeliverMessage(&outgoingMessages[localThreadId].back());
	}

	void ClearSentMessages()
	{
		unsigned int localThreadId = ProcessScheduler::Retrieve()->GetLocalThreadId();
		while (!outgoingMessages[localThreadId].empty() && outgoingMessages[localThreadId].front().processed)
		{
			outgoingMessages[localThreadId].pop();
		}
	}

	void CancelOutgoingMessages()
	{
		for (unsigned int i = 0; i < numberOfMessageSenders; ++i)
		{
			std::queue<MessageType>& outgoingQueue = outgoingMessages[i];

			while (!outgoingQueue.empty())
			{
				if (outgoingQueue.front().senderAvailable != nullptr)
				{
					*outgoingQueue.front().senderAvailable = true;
				}
				
				outgoingQueue.pop();
			}
		}
	}

private:
	unsigned int numberOfMessageSenders = 0;
	std::queue<MessageType>* outgoingMessages;
};

