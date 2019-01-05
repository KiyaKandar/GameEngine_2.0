#pragma once

#include "MessageStorage.h"

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
		sentMessages = new std::queue<MessageType>[numberOfMessageSenders];
	}

	~LockFreeThreadSafeMessageBuffer()
	{
		delete[] outgoingMessages;
		delete[] sentMessages;
	}

	void InsertOutgoingMessage(MessageType message)
	{
		outgoingMessages[ProcessScheduler::Retrieve()->GetLocalThreadId()].push(message);
	}

	void SendMessages(MessageStorage* messageStorage)
	{
		unsigned int localThreadId = ProcessScheduler::Retrieve()->GetLocalThreadId();

		std::queue<MessageType>& outgoingQueue = outgoingMessages[localThreadId];

		while (!outgoingQueue.empty())
		{
			MessageType message = outgoingQueue.front();
			outgoingQueue.pop();

			sentMessages[localThreadId].push(message);
			messageStorage->DeliverMessage(&sentMessages[localThreadId].back(), localThreadId);
		}
	}

	void ClearSentMessages()
	{
		unsigned int localThreadId = ProcessScheduler::Retrieve()->GetLocalThreadId();

		while (!sentMessages[localThreadId].empty() && sentMessages[localThreadId].front().processed)
		{
			sentMessages[localThreadId].pop();
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
	std::queue<MessageType>* sentMessages;
};

