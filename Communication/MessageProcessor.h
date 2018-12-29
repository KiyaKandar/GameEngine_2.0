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

	void addActionToExecuteOnMessage(const MessageType& typeOfMessageToPerformOn, const Action& action);
	void processMessagesInBuffer();

private:
	void getReceivedMessagesFromDeliveryBuffer(std::vector<Message*>& receivedMessages);
	void processMessageByPerformingAssignedActions(Message* message);

	MessageDeliveryBuffer* subsystemMessageBuffer;
	std::unordered_map<int, std::vector<Action>*> actionsToExecute;
};

