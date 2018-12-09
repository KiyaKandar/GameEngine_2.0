#pragma once

#include "Message.h"
#include "Messages/PlayerInputMessage.h"
#include "Messages/TextMessage.h"
#include "Messages/PlaySoundMessage.h"
#include "Messages/PlayMovingSoundMessage.h"
#include "Messages/StopSoundMessage.h"
#include <string>
#include <queue>
#include "Messages/RelativeTransformMessage.h"
#include "Messages/ToggleGraphicsModuleMessage.h"
#include "Messages/ApplyForceMessage.h"
#include "Messages/MoveCameraRelativeToGameObjectMessage.h"
#include "Messages/ApplyImpulseMessage.h"
#include "Messages/CollisionMessage.h"
#include "Messages/PreparePaintSurfaceMessage.h"
#include "Messages/PaintTrailForGameObjectMessage.h"
#include "Messages/UpdatePositionMessage.h"
#include "Messages/TextMeshMessage.h"
#include "Messages/AddScoreHolderMessage.h"
#include "Messages/AbsoluteTransformMessage.h"
#include "Messages/MoveGameObjectMessage.h"
#include "Messages/ScaleGameObjectMessage.h"
#include "Messages/RotateGameObjectMessage.h"
#include "Messages/ToggleGameObjectMessage.h"
#include "Messages/PlayAnimationMessage.h"
#include "Messages/DebugLineMessage.h"
#include "Messages/DebugSphereMessage.h"

//exposed interface
class MessagingService 
{
public:
	virtual void addDeliveryPoint(const std::string& bufferName) = 0;
	virtual std::queue<Message*>* getDeliveryPoint(const std::string& bufferName) = 0;

	virtual void insertMessage(Message message) = 0;
	virtual void insertMessage(PlayerInputMessage message) = 0;
	virtual void insertMessage(TextMessage message) = 0;
	virtual void insertMessage(RelativeTransformMessage message) = 0;	
	virtual void insertMessage(PlaySoundMessage message) = 0;
	virtual void insertMessage(PlayMovingSoundMessage message) = 0;
	virtual void insertMessage(StopSoundMessage message) = 0;
	virtual void insertMessage(ToggleGraphicsModuleMessage message) = 0;
	virtual void insertMessage(ApplyForceMessage message) = 0;
	virtual void insertMessage(ApplyImpulseMessage message) = 0;
	virtual void insertMessage(MoveCameraRelativeToGameObjectMessage message) = 0;
	virtual void insertMessage(CollisionMessage message) = 0;
	virtual void insertMessage(PreparePaintSurfaceMessage message) = 0;
	virtual void insertMessage(PaintTrailForGameObjectMessage message) = 0;
	virtual void insertMessage(UpdatePositionMessage message) = 0;
	virtual void insertMessage(TextMeshMessage message) = 0;
	virtual void insertMessage(AddScoreHolderMessage message) = 0;
	virtual void insertMessage(AbsoluteTransformMessage message) = 0;
	virtual void insertMessage(MoveGameObjectMessage message) = 0;
	virtual void insertMessage(ScaleGameObjectMessage message) = 0;
	virtual void insertMessage(RotateGameObjectMessage message) = 0;
	virtual void insertMessage(ToggleGameObjectMessage message) = 0;
	virtual void insertMessage(PlayAnimationMessage message) = 0;
	virtual void insertMessage(DebugLineMessage message) = 0;
	virtual void insertMessage(DebugSphereMessage message) = 0;

	virtual void deliverAllMessages() = 0;
	virtual void clearAllMessages() = 0;
	virtual void cancelOutgoingMessages() = 0;
};