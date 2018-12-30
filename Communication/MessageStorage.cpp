#include "MessageStorage.h"

#include "Message.h"
#include "MessageDeliveryBuffer.h"

MessageStorage::MessageStorage()
{

}

MessageStorage::~MessageStorage() 
{
}

void MessageStorage::AddMessageBuffer(const std::string& bufferName)
{
	activeMessageBuffers.insert(std::pair<std::string, MessageDeliveryBuffer*>(bufferName, new MessageDeliveryBuffer()));
}

void MessageStorage::DeliverMessage(Message* message, unsigned int threadId)
{
	activeMessageBuffers.at(message->GetDestination())->Push(message, threadId);
}

MessageDeliveryBuffer* MessageStorage::GetMessageBufferByName(const std::string& bufferName)
{
	return activeMessageBuffers.at(bufferName);
}

void MessageStorage::ClearMessageStorage()
{
	for (auto iter = activeMessageBuffers.begin(); iter != activeMessageBuffers.end(); iter++)
	{
		iter->second->ClearAll();
	}
}