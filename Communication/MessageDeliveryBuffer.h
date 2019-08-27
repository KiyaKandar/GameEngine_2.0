#pragma once

#include "Launch/Threading/Scheduler/ProcessScheduler.h"
#include "LockFreeQueue.h"

#include <deque>
#include <mutex>

class Message;

class MessageDeliveryBuffer
{
public:
	MessageDeliveryBuffer()
	{
		buffer = moodycamel::ConcurrentQueue<Message*>(maxNumMessages);
	}

	void Push(Message* message)
	{
		if (!buffer.try_enqueue(message))
		{
			message->processed = true;
		}
	}

	void ClearAll()
	{
		const size_t approximateNumCurrentlyReceivedMessages = buffer.size_approx();

		for (size_t i = 0; i < approximateNumCurrentlyReceivedMessages; ++i)
		{
			Message* message;
			buffer.try_dequeue(message);
		}
	}

	const size_t maxNumMessages = 5000;
	moodycamel::ConcurrentQueue<Message*> buffer;
};
