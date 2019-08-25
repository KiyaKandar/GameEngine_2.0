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

	void AddMessageBuffer(const std::string& bufferName);
	MessageDeliveryBuffer* GetMessageBufferByName(const std::string& bufferName);

	void DeliverMessage(Message* message);
	void ClearMessageStorage();

private:
	std::map<std::string, MessageDeliveryBuffer*> activeMessageBuffers;
};
