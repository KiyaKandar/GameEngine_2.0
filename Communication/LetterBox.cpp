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
	messageStorage->AddMessageBuffer(bufferName);
}

MessageDeliveryBuffer* LetterBox::GetDeliveryPoint(const std::string& bufferName)
{
	return messageStorage->GetMessageBufferByName(bufferName);
}
