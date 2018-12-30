#pragma once

#include "../DeliverySystem.h"

template <class MessageType>
class TrackedGroupMessageSender
{
public:
	TrackedGroupMessageSender()
	{
	}

	~TrackedGroupMessageSender()
	{

	}

	void SetMessageGroup(const std::vector<MessageType>& messages)
	{
		this->messages = messages;
	}

	void SendMessageGroup()
	{
		if (ReadyToSendNextMessageGroup())
		{
			trackers.clear();
			trackers.resize(messages.size(), false);

			for (int i = 0; i < messages.size(); ++i)
			{
				messages[i].senderAvailable = &trackers[i];
				DeliverySystem::GetPostman()->InsertMessage(messages[i]);
			}
		}
	}

	bool ReadyToSendNextMessageGroup() const
	{
		bool canSend = true;
		for (const bool tracker : trackers)
		{
			canSend = canSend && tracker;
		}

		return canSend;
	}

private:
	std::vector<MessageType> messages;
	std::deque<bool> trackers;
};