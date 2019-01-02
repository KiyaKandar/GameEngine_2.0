#pragma once

#include "../DeliverySystem.h"

template <class MessageType>
class TrackedMessageSender
{
public:
	TrackedMessageSender()
	{
		lastSentMessageProcessed = true;
	}

	~TrackedMessageSender()
	{

	}

	void SetMessage(const MessageType& newMessage)
	{
		message = newMessage;
		message.senderAvailable = &lastSentMessageProcessed;
	}

	void SendTrackedMessage()
	{
		if (lastSentMessageProcessed)
		{
			lastSentMessageProcessed = false;
			DeliverySystem::GetPostman()->InsertMessage(message);
		}
	}

	bool ReadyToSendNextMessage() const
	{
		return lastSentMessageProcessed;
	}

private:
	MessageType message;
	bool lastSentMessageProcessed;
};