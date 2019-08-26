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
#include "Messages/UIQuadBatchMessage.h"
#include "MessageSenders/TrackedMessageSender.h"

class LetterBox : public MessagingService
{
public:
	LetterBox();
	virtual ~LetterBox();

	void AddDeliveryPoint(const std::string& bufferName) override;
	MessageDeliveryBuffer* GetDeliveryPoint(const std::string& bufferName) override;
	const std::vector<std::string>& GetAllDeliveryPoints() const override;

	REGISTER_ALL_MESSAGES()
	REGISTER_ALL_TRACKED_SENDERS()

	void DeliverAllMessages() override
	{
		REGISTER_ALL_SENDER()
	}

	void ClearAllMessages()
	{
		REGISTER_ALL_CLEAR()
	}

	void CancelOutgoingMessages()
	{
		REGISTER_ALL_CANCEL()
	}

	void CancelDeliveredMessages()
	{
		messageStorage->ClearMessageStorage();
	}

	void DeleteAllTrackedSenders()
	{
		REGISTER_ALL_TRACKED_SENDERS_DELETION()
	}

private:
	MessageStorage* messageStorage;
	std::vector<std::string> deliveryPoints;
};
