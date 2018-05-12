#include "MessageStorage.h"

MessageStorage::MessageStorage()
{

}

MessageStorage::~MessageStorage() 
{
}

void MessageStorage::addMessageBuffer(const std::string& bufferName)
{
	activeMessageBuffers.insert(std::pair<std::string, std::queue<Message*>>(bufferName, std::queue<Message*> ()));
}

void MessageStorage::removeMessageBuffer(const std::string& bufferName)
{
	clearMessageBuffer(bufferName);
	activeMessageBuffers.erase(bufferName);
}

void MessageStorage::sendMessage(Message* message)
{
	activeMessageBuffers.at(message->getDestination()).push(message);
}

std::queue<Message*>* MessageStorage::getMessageBufferByName(const std::string& bufferName)
{
	return &activeMessageBuffers.at(bufferName);
}

void MessageStorage::clearMessageStorage()
{
	for (auto iter = activeMessageBuffers.begin(); iter != activeMessageBuffers.end(); iter++)
	{
		clearMessageBuffer(iter);
	}
}

void MessageStorage::clearMessageBuffer(const std::string& bufferName)
{
	std::queue<Message*> buffer = activeMessageBuffers.at(bufferName);
	while (!buffer.empty())
	{
		buffer.pop();
	}
}

void MessageStorage::clearMessageBuffer(std::map<std::string, std::queue<Message*>>::iterator iter)
{
	while (!iter->second.empty())
	{
		iter->second.pop();
	}
}