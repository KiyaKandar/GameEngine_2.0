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
	deliveryPoints.push_back(bufferName);
}

std::queue<Message*>* LetterBox::getDeliveryPoint(const std::string& bufferName)
{
	return messageStorage->getMessageBufferByName(bufferName);
}

const std::vector<std::string>& LetterBox::getAllDeliveryPoints() const
{
	return deliveryPoints;
}
