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
