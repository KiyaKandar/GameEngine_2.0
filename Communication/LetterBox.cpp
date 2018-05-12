#include "LetterBox.h"
#include "DeliverySystem.h"

LetterBox::LetterBox()
{
	messageStorage = new MessageStorage();
}

LetterBox::~LetterBox()
{
	delete messageStorage;
}

void LetterBox::addDeliveryPoint(const std::string& bufferName)
{
	messageStorage->addMessageBuffer(bufferName);
}

std::queue<Message*>* LetterBox::getDeliveryPoint(const std::string& bufferName)
{
	return messageStorage->getMessageBufferByName(bufferName);
}

void LetterBox::insertMessage(Message message)
{
	messageBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(PlayerInputMessage message)
{
	playerInputMessageBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(TextMessage message)
{
	textMessageBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(RelativeTransformMessage message)
{
	relativeTransformMessageBuffer.insertOutgoingMessage(message);
}
void LetterBox::insertMessage(PlaySoundMessage message)
{
	playSoundMessageBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(StopSoundMessage message)
{
	stopSoundMessageBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(ToggleGraphicsModuleMessage message)
{
	toggleGraphicsModuleMessageBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(ApplyForceMessage message)
{
	applyForceMessageBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(MoveCameraRelativeToGameObjectMessage message)
{
	moveCameraBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(ApplyImpulseMessage message)
{
	applyImpulseMessageBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(AbsoluteTransformMessage message)
{
	absoluteTransformBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(MoveGameObjectMessage message)
{
	moveGameObjectBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(ScaleGameObjectMessage message)
{
	scaleGameObjectBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(RotateGameObjectMessage message)
{
	rotateGameObjectBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(ToggleGameObjectMessage message)
{
	toggleGameObjectBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(CollisionMessage message)
{
	collisionBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(PreparePaintSurfaceMessage message)
{
	preparePaintSurfaceBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(PaintTrailForGameObjectMessage message)
{
	paintTrailForGameObjectBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(UpdatePositionMessage message)
{
	updatePositionBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(TextMeshMessage message)
{
	textMeshBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(AddScoreHolderMessage message)
{
	scoreBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(PlayMovingSoundMessage message)
{
	playMovingSoundMessageBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(DebugLineMessage message)
{
	debugLineBuffer.insertOutgoingMessage(message);
}

void LetterBox::insertMessage(DebugSphereMessage message)
{
	debugCircleBuffer.insertOutgoingMessage(message);
}

void LetterBox::deliverAllMessages()
{
	messageBuffer.sendMessages(messageStorage);
	playerInputMessageBuffer.sendMessages(messageStorage);
	textMessageBuffer.sendMessages(messageStorage);
	relativeTransformMessageBuffer.sendMessages(messageStorage);
	playSoundMessageBuffer.sendMessages(messageStorage);
	stopSoundMessageBuffer.sendMessages(messageStorage);
	toggleGraphicsModuleMessageBuffer.sendMessages(messageStorage);
	applyForceMessageBuffer.sendMessages(messageStorage);
	moveCameraBuffer.sendMessages(messageStorage);
	collisionBuffer.sendMessages(messageStorage);
	preparePaintSurfaceBuffer.sendMessages(messageStorage);
	applyImpulseMessageBuffer.sendMessages(messageStorage);
	paintTrailForGameObjectBuffer.sendMessages(messageStorage);
	updatePositionBuffer.sendMessages(messageStorage);
	textMeshBuffer.sendMessages(messageStorage);
	scoreBuffer.sendMessages(messageStorage);
	absoluteTransformBuffer.sendMessages(messageStorage);
	moveGameObjectBuffer.sendMessages(messageStorage);
	scaleGameObjectBuffer.sendMessages(messageStorage);
	rotateGameObjectBuffer.sendMessages(messageStorage);
	playMovingSoundMessageBuffer.sendMessages(messageStorage);
	toggleGameObjectBuffer.sendMessages(messageStorage);
	debugLineBuffer.sendMessages(messageStorage);
	debugCircleBuffer.sendMessages(messageStorage);
}

void LetterBox::clearAllMessages()
{
	messageBuffer.clearSentMessages();
	playerInputMessageBuffer.clearSentMessages();
	textMessageBuffer.clearSentMessages();
	relativeTransformMessageBuffer.clearSentMessages();
	playSoundMessageBuffer.clearSentMessages();
	stopSoundMessageBuffer.clearSentMessages();
	toggleGraphicsModuleMessageBuffer.clearSentMessages();
	applyForceMessageBuffer.clearSentMessages();
	applyImpulseMessageBuffer.clearSentMessages();
	moveCameraBuffer.clearSentMessages();
	collisionBuffer.clearSentMessages();
	preparePaintSurfaceBuffer.clearSentMessages();
	paintTrailForGameObjectBuffer.clearSentMessages();
	updatePositionBuffer.clearSentMessages();
	textMeshBuffer.clearSentMessages();
	scoreBuffer.clearSentMessages();
	absoluteTransformBuffer.clearSentMessages();
	moveGameObjectBuffer.clearSentMessages();
	scaleGameObjectBuffer.clearSentMessages();
	rotateGameObjectBuffer.clearSentMessages();
	playMovingSoundMessageBuffer.clearSentMessages();
	debugLineBuffer.clearSentMessages();
	debugCircleBuffer.clearSentMessages();

	
	toggleGameObjectBuffer.clearSentMessages();
	messageStorage->clearMessageStorage();
}

void LetterBox::cancelOutgoingMessages()
{
	messageBuffer.clearOutgoingMessages();
	playerInputMessageBuffer.clearOutgoingMessages();
	textMessageBuffer.clearOutgoingMessages();
	relativeTransformMessageBuffer.clearOutgoingMessages();
	playSoundMessageBuffer.clearOutgoingMessages();
	stopSoundMessageBuffer.clearOutgoingMessages();
	toggleGraphicsModuleMessageBuffer.clearOutgoingMessages();
	applyForceMessageBuffer.clearOutgoingMessages();
	applyImpulseMessageBuffer.clearOutgoingMessages();
	moveCameraBuffer.clearOutgoingMessages();
	collisionBuffer.clearOutgoingMessages();
	preparePaintSurfaceBuffer.clearOutgoingMessages();
	paintTrailForGameObjectBuffer.clearOutgoingMessages();
	updatePositionBuffer.clearOutgoingMessages();
	textMeshBuffer.clearOutgoingMessages();
	scoreBuffer.clearOutgoingMessages();
	absoluteTransformBuffer.clearOutgoingMessages();
	moveGameObjectBuffer.clearOutgoingMessages();
	scaleGameObjectBuffer.clearOutgoingMessages();
	rotateGameObjectBuffer.clearOutgoingMessages();
	playMovingSoundMessageBuffer.clearOutgoingMessages();
	toggleGameObjectBuffer.clearOutgoingMessages();
	debugLineBuffer.clearOutgoingMessages();
	debugCircleBuffer.clearOutgoingMessages();

}
