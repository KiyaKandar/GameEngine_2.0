#pragma once
#include <map>
#include <queue>
#include <string>
#include "Message.h"

class MessageStorage
{
public:
	MessageStorage();
	~MessageStorage();

	void addMessageBuffer(const std::string& bufferName);
	void removeMessageBuffer(const std::string& bufferName);
	std::queue<Message*>* getMessageBufferByName(const std::string& bufferName);

	void sendMessage(Message* message);
	void clearMessageStorage();

private:
	void clearMessageBuffer(const std::string& bufferName);
	void clearMessageBuffer(std::map<std::string, std::queue<Message*>>::iterator iter);

	std::map<std::string, std::queue<Message*>> activeMessageBuffers;
};
