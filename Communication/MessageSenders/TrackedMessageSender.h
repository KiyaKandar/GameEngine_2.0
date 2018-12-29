#pragma once

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

	void setMessage(const MessageType& newMessage)
	{
		message = newMessage;
		message.senderAvailable = &lastSentMessageProcessed;
	}

	void sendMessage()
	{
		if (lastSentMessageProcessed)
		{
			lastSentMessageProcessed = false;
			DeliverySystem::getPostman()->insertMessage(message);
		}
	}

	bool readyToSendNextMessage() const
	{
		return lastSentMessageProcessed;
	}

private:
	MessageType message;
	bool lastSentMessageProcessed;
};