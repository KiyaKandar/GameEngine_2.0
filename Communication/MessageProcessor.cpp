#include "MessageProcessor.h"
#include "MessageStorage.h"
#include "Messages/DummyWorkMessage.h"

MessageProcessor::MessageProcessor(std::vector<MessageType> typeOfMessagesToListenFor, 
	std::queue<Message*>* subsystemBuffer)
{
	subsystemMessageBuffer = subsystemBuffer;

	for (MessageType messageType : typeOfMessagesToListenFor)
	{
		actionsToExecute[messageType] = new std::vector<Action>();
	}

	AddDefaultMessageActions();
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

void MessageProcessor::AddDefaultMessageActions()
{
	actionsToExecute[DUMMY_WORK] = new std::vector<Action>();
	actionsToExecute.at(DUMMY_WORK)->push_back([](Message* message)
	{
		DummyWorkMessage* dummyWorkMessage = static_cast<DummyWorkMessage*>(message);
		dummyWorkMessage->DoDummyWork();
	});
}