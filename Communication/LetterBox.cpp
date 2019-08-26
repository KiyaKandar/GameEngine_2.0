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

void LetterBox::AddDeliveryPoint(const std::string& bufferName)
{
	messageStorage->addMessageBuffer(bufferName);
	deliveryPoints.push_back(bufferName);
}

MessageDeliveryBuffer* LetterBox::GetDeliveryPoint(const std::string& bufferName)
{
	return messageStorage->GetMessageBufferByName(bufferName);
}

const std::vector<std::string>& LetterBox::getAllDeliveryPoints() const
{
	return deliveryPoints;
}
