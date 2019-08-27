#include "MessageProcessor.h"
#include "Messages/DummyWorkMessage.h"

#include "MessageDeliveryBuffer.h"

MessageProcessor::MessageProcessor(std::vector<MessageType> typeOfMessagesToListenFor, 
	MessageDeliveryBuffer* subsystemBuffer)
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

void MessageProcessor::AddActionToExecuteOnMessage(const MessageType& typeOfMessageToPerformOn, 
	const Action& action)
{
	actionsToExecute.at(typeOfMessageToPerformOn)->push_back(action);
}

void MessageProcessor::ProcessMessagesInBuffer()
{
	const size_t approximateNumCurrentlyReceivedMessages = subsystemMessageBuffer->buffer.size_approx();

	for (size_t i = 0; i < approximateNumCurrentlyReceivedMessages; ++i)
	{
		Message* message;
		subsystemMessageBuffer->buffer.try_dequeue(message);

		if (message)
		{
			ProcessMessageByPerformingAssignedActions(message);
		}
	}
}

void MessageProcessor::ProcessMessageByPerformingAssignedActions(Message* message)
{
	std::vector<Action>& actionsForMessageType = *actionsToExecute.at(message->GetMessageType());

	for (Action& action : actionsForMessageType)
	{
		action(message);
	}

	message->processed = true;

	if (message->senderAvailable != nullptr)
	{
		*message->senderAvailable = true;
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