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

void MessageProcessor::addActionToExecuteOnMessage(const MessageType& typeOfMessageToPerformOn, 
	const Action& action)
{
	actionsToExecute.at(typeOfMessageToPerformOn)->push_back(action);
}

void MessageProcessor::processMessagesInBuffer()
{
	std::vector<Message*> receivedMessages;
	getReceivedMessagesFromDeliveryBuffer(receivedMessages);

	for (Message* message : receivedMessages)
	{
		processMessageByPerformingAssignedActions(message);
	}
	
	receivedMessages.clear();
}

void MessageProcessor::getReceivedMessagesFromDeliveryBuffer(std::vector<Message*>& receivedMessages)
{
	unsigned int numberOfThreadSenders = ThreadPool::getTotalNumberOfThreads();

	for (int threadId = 0; threadId < numberOfThreadSenders; ++threadId)
	{
		const unsigned int numberOfMessagesReceivedFromThread = subsystemMessageBuffer->count(threadId);

		for (int messageNum = 0; messageNum < numberOfMessagesReceivedFromThread; ++messageNum)
		{
			receivedMessages.push_back(subsystemMessageBuffer->read(threadId, messageNum));
		}

		subsystemMessageBuffer->clear(numberOfMessagesReceivedFromThread, threadId);
	}
}

void MessageProcessor::processMessageByPerformingAssignedActions(Message * message)
{
	std::vector<Action>& actionsForMessageType = *actionsToExecute.at(message->getMessageType());

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
