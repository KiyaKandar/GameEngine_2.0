#include "MessageStorage.h"

#include "Message.h"
#include "MessageDeliveryBuffer.h"

MessageStorage::MessageStorage()
{

}

MessageStorage::~MessageStorage() 
{
}

void MessageStorage::addMessageBuffer(const std::string& bufferName)
{
	activeMessageBuffers.insert(std::pair<std::string, MessageDeliveryBuffer*>(bufferName, new MessageDeliveryBuffer()));
}

void MessageStorage::deliverMessage(Message* message, unsigned int threadId)
{
	activeMessageBuffers.at(message->getDestination())->push(message, threadId);
}

MessageDeliveryBuffer* MessageStorage::getMessageBufferByName(const std::string& bufferName)
{
	return activeMessageBuffers.at(bufferName);
}

void MessageStorage::clearMessageStorage()
{
	for (auto iter = activeMessageBuffers.begin(); iter != activeMessageBuffers.end(); iter++)
	{
		iter->second->clearAll();
	}
}