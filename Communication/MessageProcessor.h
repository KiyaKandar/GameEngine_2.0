#pragma once

#include "Message.h"

#include <queue>
#include <functional>
#include <unordered_map>

typedef std::function<void(Message*)> Action;

class MessageDeliveryBuffer;

class MessageProcessor
{
public:
	MessageProcessor() {}
	MessageProcessor(std::vector<MessageType> typeOfMessagesToListenFor, 
		MessageDeliveryBuffer* subsystemBuffer);
	~MessageProcessor();

	void AddActionToExecuteOnMessage(const MessageType& typeOfMessageToPerformOn, const Action& action);
	void ProcessMessagesInBuffer();

private:
	void ProcessMessageByPerformingAssignedActions(Message* message);
	void AddDefaultMessageActions();

	MessageDeliveryBuffer* subsystemMessageBuffer;
	std::unordered_map<int, std::vector<Action>*> actionsToExecute;
};

