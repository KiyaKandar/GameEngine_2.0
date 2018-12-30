#include "MessageProcessor.h"

#include "MessageDeliveryBuffer.h"
#include "../Launch/Threading/ThreadPool/ThreadPool.h"

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
	std::vector<Message*> receivedMessages;
	GetReceivedMessagesFromDeliveryBuffer(receivedMessages);

	for (Message* message : receivedMessages)
	{
		ProcessMessageByPerformingAssignedActions(message);
	}
	
	receivedMessages.clear();
}

void MessageProcessor::GetReceivedMessagesFromDeliveryBuffer(std::vector<Message*>& receivedMessages)
{
	unsigned int numberOfThreadSenders = ThreadPool::GetTotalNumberOfThreads();

	for (int threadId = 0; threadId < numberOfThreadSenders; ++threadId)
	{
		const unsigned int numberOfMessagesReceivedFromThread = subsystemMessageBuffer->Count(threadId);

		for (int messageNum = 0; messageNum < numberOfMessagesReceivedFromThread; ++messageNum)
		{
			receivedMessages.push_back(subsystemMessageBuffer->Read(threadId, messageNum));
		}

		subsystemMessageBuffer->Clear(numberOfMessagesReceivedFromThread, threadId);
	}
}

void MessageProcessor::ProcessMessageByPerformingAssignedActions(Message * message)
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
