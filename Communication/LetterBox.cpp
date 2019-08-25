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

void LetterBox::AddDeliveryPoint(const std::string& bufferName)
{
	messageStorage->AddMessageBuffer(bufferName);
}

MessageDeliveryBuffer* LetterBox::GetDeliveryPoint(const std::string& bufferName)
{
	return messageStorage->GetMessageBufferByName(bufferName);
}
