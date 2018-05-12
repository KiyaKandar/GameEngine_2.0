#pragma once

#include "Message.h"
#include <queue>
#include <functional>
#include <unordered_map>

typedef std::function<void(Message*)> Action;

class MessageProcessor
{
public:
	MessageProcessor() {}
	MessageProcessor(std::vector<MessageType> typeOfMessagesToListenFor, 
		std::queue<Message*>* subsystemBuffer);
	~MessageProcessor();

	void addActionToExecuteOnMessage(const MessageType& typeOfMessageToPerformOn, const Action& action);
	void processMessagesInBuffer();

private:
	std::queue<Message*>* subsystemMessageBuffer;
	std::unordered_map<int, std::vector<Action>*> actionsToExecute;
};

