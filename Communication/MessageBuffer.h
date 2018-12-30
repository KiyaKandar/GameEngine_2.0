#pragma once

#include "MessageStorage.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

template <class MessageType>
class MessageBuffer
{
public:
	MessageBuffer() {}
	~MessageBuffer() {}

	void InsertOutgoingMessage(MessageType message)
	{
		outgoingMessages.push(message);
	}

	void SendMessages(MessageStorage* messageStorage)
	{
		while (!outgoingMessages.empty())
		{
			MessageType message = outgoingMessages.front();
			outgoingMessages.pop();

			sentMessages.push(message);
			messageStorage->DeliverMessage(&sentMessages.back());
		}
	}

	void ClearSentMessages()
	{
		while (!sentMessages.empty())
		{
			sentMessages.pop();
		}
	}

	void ClearOutgoingMessages()
	{
		while (!outgoingMessages.empty())
		{
			outgoingMessages.pop();
		}
	}

private:
	std::queue<MessageType> outgoingMessages;
	std::queue<MessageType> sentMessages;
};

