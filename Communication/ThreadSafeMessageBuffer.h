#pragma once
#include "MessageBuffer.h"

template <class MessageType>
/*
	THIS IS DEPRECATED NOW.
	USE LockFreeThreadSafeMessageBufer INSTEAD.
*/
class ThreadSafeMessageBuffer
{
public:
	ThreadSafeMessageBuffer() {}
	~ThreadSafeMessageBuffer() {}

	void insertOutgoingMessage(MessageType message)
	{
		std::unique_lock<std::mutex> lock(bufferLock);
		messageBuffer.insertOutgoingMessage(message);
	}

	void sendMessages(MessageStorage* messageStorage)
	{
		messageBuffer.sendMessages(messageStorage);
	}

	void clearSentMessages()
	{
		messageBuffer.clearSentMessages();
	}

	void clearOutgoingMessages()
	{
		messageBuffer.clearOutgoingMessages();
	}

private:
	MessageBuffer<MessageType> messageBuffer;
	std::mutex bufferLock;
};

