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
#include "MessageRegistry.h"

//exposed interface
class MessagingService 
{
public:
	virtual void addDeliveryPoint(const std::string& bufferName) = 0;
	virtual std::queue<Message*>* getDeliveryPoint(const std::string& bufferName) = 0;

	REGISTER_PURE_VIRTUAL_SERVICE()

	virtual void deliverAllMessages() = 0;
	virtual void clearAllMessages() = 0;
	virtual void cancelOutgoingMessages() = 0;
};