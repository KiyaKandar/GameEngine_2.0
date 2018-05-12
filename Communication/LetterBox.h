#pragma once

#include "Message.h"
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
#include "ThreadSafeMessageBuffer.h"
#include "Messages/DebugLineMessage.h"

class LetterBox : public MessagingService
{
public:
	LetterBox();
	virtual ~LetterBox();

	void addDeliveryPoint(const std::string& bufferName) override;
	std::queue<Message*>* getDeliveryPoint(const std::string& bufferName) override;

	void insertMessage(Message message) override;
	void insertMessage(PlayerInputMessage message) override;
	void insertMessage(TextMessage message) override;
	void insertMessage(RelativeTransformMessage message) override;	
	void insertMessage(PlaySoundMessage message) override;
	void insertMessage(PlayMovingSoundMessage message) override;
	void insertMessage(StopSoundMessage message) override;
	void insertMessage(ToggleGraphicsModuleMessage message) override;
	void insertMessage(ApplyForceMessage message) override;
	void insertMessage(MoveCameraRelativeToGameObjectMessage message) override;
	void insertMessage(CollisionMessage message) override;
	void insertMessage(PreparePaintSurfaceMessage message) override;
	void insertMessage(PaintTrailForGameObjectMessage message) override;
	void insertMessage(UpdatePositionMessage message) override;
	void insertMessage(TextMeshMessage message) override;
	void insertMessage(AddScoreHolderMessage message) override;
	void insertMessage(ApplyImpulseMessage message) override;
	void insertMessage(AbsoluteTransformMessage message) override;
	void insertMessage(MoveGameObjectMessage message) override;
	void insertMessage(ScaleGameObjectMessage message) override;
	void insertMessage(RotateGameObjectMessage message) override;
	void insertMessage(ToggleGameObjectMessage message) override;
	void insertMessage(DebugLineMessage message) override;
	void insertMessage(DebugSphereMessage message) override;

	void deliverAllMessages() override;
	void clearAllMessages() override;
	void cancelOutgoingMessages() override;

private:
	MessageStorage* messageStorage;

	MessageBuffer<Message> messageBuffer;
	MessageBuffer<PlayerInputMessage> playerInputMessageBuffer;
	MessageBuffer<TextMessage> textMessageBuffer;
	MessageBuffer<RelativeTransformMessage> relativeTransformMessageBuffer;
	MessageBuffer<PlaySoundMessage> playSoundMessageBuffer;
	MessageBuffer<PlayMovingSoundMessage> playMovingSoundMessageBuffer;
	MessageBuffer<StopSoundMessage> stopSoundMessageBuffer;
	MessageBuffer<ToggleGraphicsModuleMessage> toggleGraphicsModuleMessageBuffer;
	MessageBuffer<ApplyForceMessage> applyForceMessageBuffer;
	MessageBuffer<ApplyImpulseMessage> applyImpulseMessageBuffer;
	MessageBuffer<MoveCameraRelativeToGameObjectMessage> moveCameraBuffer;
	MessageBuffer<CollisionMessage> collisionBuffer;
	MessageBuffer<PreparePaintSurfaceMessage> preparePaintSurfaceBuffer;
	MessageBuffer<PaintTrailForGameObjectMessage> paintTrailForGameObjectBuffer;
	MessageBuffer<UpdatePositionMessage> updatePositionBuffer;
	ThreadSafeMessageBuffer<TextMeshMessage> textMeshBuffer;
	MessageBuffer<AddScoreHolderMessage> scoreBuffer;
	MessageBuffer<AbsoluteTransformMessage> absoluteTransformBuffer;
	MessageBuffer<MoveGameObjectMessage> moveGameObjectBuffer;
	MessageBuffer<ScaleGameObjectMessage> scaleGameObjectBuffer;
	MessageBuffer<RotateGameObjectMessage> rotateGameObjectBuffer;
	MessageBuffer<ToggleGameObjectMessage> toggleGameObjectBuffer;
	MessageBuffer<DebugLineMessage> debugLineBuffer;
	MessageBuffer<DebugSphereMessage> debugCircleBuffer;
};

