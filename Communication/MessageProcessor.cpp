#include "MessageProcessor.h"

#include "MessageDeliveryBuffer.h"

MessageProcessor::MessageProcessor(std::vector<MessageType> typeOfMessagesToListenFor, 
	MessageDeliveryBuffer* subsystemBuffer)
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
