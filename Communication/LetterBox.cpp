#include "LetterBox.h"
#include "DeliverySystem.h"

INITIALISE_GLOBAL_TRACKED_SENDERS(LetterBox)

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

MessageDeliveryBuffer* LetterBox::getDeliveryPoint(const std::string& bufferName)
{
	return messageStorage->getMessageBufferByName(bufferName);
}
