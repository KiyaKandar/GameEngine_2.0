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

	void InsertOutgoingMessage(MessageType message)
	{
		std::unique_lock<std::mutex> lock(bufferLock);
		messageBuffer.insertOutgoingMessage(message);
	}

	void SendMessages(MessageStorage* messageStorage)
	{
		std::unique_lock<std::mutex> lock(bufferLock);
		messageBuffer.sendMessages(messageStorage);
	}

	void ClearSentMessages()
	{
		std::unique_lock<std::mutex> lock(bufferLock);
		messageBuffer.clearSentMessages();
	}

	void ClearOutgoingMessages()
	{
		std::unique_lock<std::mutex> lock(bufferLock);
		messageBuffer.clearOutgoingMessages();
	}

private:
	MessageBuffer<MessageType> messageBuffer;
	std::mutex bufferLock;
};

