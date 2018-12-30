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
	void GetReceivedMessagesFromDeliveryBuffer(std::vector<Message*>& receivedMessages);
	void ProcessMessageByPerformingAssignedActions(Message* message);

	MessageDeliveryBuffer* subsystemMessageBuffer;
	std::unordered_map<int, std::vector<Action>*> actionsToExecute;
};

