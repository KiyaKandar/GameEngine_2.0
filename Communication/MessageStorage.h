#pragma once

#include <map>
#include <string>

class MessageDeliveryBuffer;
class Message;

class MessageStorage
{
public:
	MessageStorage();
	~MessageStorage();

	void addMessageBuffer(const std::string& bufferName);
	MessageDeliveryBuffer* getMessageBufferByName(const std::string& bufferName);

	void deliverMessage(Message* message, unsigned int threadId);
	void clearMessageStorage();

private:
	std::map<std::string, MessageDeliveryBuffer*> activeMessageBuffers;
};
