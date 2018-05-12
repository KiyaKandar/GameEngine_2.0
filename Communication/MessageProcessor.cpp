#include "MessageProcessor.h"
#include "MessageStorage.h"

MessageProcessor::MessageProcessor(std::vector<MessageType> typeOfMessagesToListenFor, 
	std::queue<Message*>* subsystemBuffer)
{
	subsystemMessageBuffer = subsystemBuffer;

	for (MessageType messageType : typeOfMessagesToListenFor)
	{
		actionsToExecute[messageType] = new std::vector<Action>();
	}
}

MessageProcessor::~MessageProcessor()
{
}

void MessageProcessor::addActionToExecuteOnMessage(const MessageType& typeOfMessageToPerformOn, 
	const Action& action)
{
	actionsToExecute.at(typeOfMessageToPerformOn)->push_back(action);
}

void MessageProcessor::processMessagesInBuffer()
{
	while (!subsystemMessageBuffer->empty())
	{
		Message* message = subsystemMessageBuffer->front();

		for (Action action : *actionsToExecute.at(message->getMessageType()))
		{
			action(message);
		}

		subsystemMessageBuffer->pop();
	}
}
