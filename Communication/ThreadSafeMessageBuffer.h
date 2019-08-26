#pragma once
#include "MessageBuffer.h"

template <class MessageType>
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
		std::unique_lock<std::mutex> lock(bufferLock);
		messageBuffer.sendMessages(messageStorage);
	}

	void clearSentMessages()
	{
		std::unique_lock<std::mutex> lock(bufferLock);
		messageBuffer.clearSentMessages();
	}

	void clearOutgoingMessages()
	{
		std::unique_lock<std::mutex> lock(bufferLock);
		messageBuffer.clearOutgoingMessages();
	}

private:
	MessageBuffer<MessageType> messageBuffer;
	std::mutex bufferLock;
};

