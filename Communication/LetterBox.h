#pragma once

#include "Message.h"

#include "MessageRegistry.h"
#include "Messages/PlayerInputMessage.h"
#include "Messages/TextMessage.h"
#include "Messages/PlaySoundMessage.h"
#include "Messages/StopSoundMessage.h"
#include "Messages/MoveCameraRelativeToGameObjectMessage.h"
#include "Messages/PlayMovingSoundMessage.h"
#include "MessagingService.h"
#include "MessageStorage.h"
#include "Messages/ApplyForceMessage.h"
#include "MessageBuffer.h"
#include "Messages/ApplyImpulseMessage.h"
#include "Messages/PaintTrailForGameObjectMessage.h"
#include "Messages/UpdatePositionMessage.h"
#include "Messages/ToggleGameObjectMessage.h"
#include "Messages/PlayAnimationMessage.h"
#include "LockFreeThreadSafeMessageBuffer.h"
#include "Messages/DebugLineMessage.h"
#include "MessageSenders/TrackedMessageSender.h"

class LetterBox : public MessagingService
{
public:
	LetterBox();
	virtual ~LetterBox();

	void addDeliveryPoint(const std::string& bufferName) override;
	MessageDeliveryBuffer* getDeliveryPoint(const std::string& bufferName) override;

	REGISTER_ALL_MESSAGES()
	REGISTER_ALL_TRACKED_SENDERS()

	void deliverAllMessages() override
	{
		REGISTER_ALL_SENDER()
	}

	void clearAllMessages()
	{
		REGISTER_ALL_CLEAR()
	}

	void cancelOutgoingMessages()
	{
		REGISTER_ALL_CANCEL()
	}

	void cancelDeliveredMessages()
	{
		messageStorage->clearMessageStorage();
	}

	void deleteAllTrackedSenders()
	{
		REGISTER_ALL_TRACKED_SENDERS_DELETION()
	}

private:
	MessageStorage* messageStorage;
};
