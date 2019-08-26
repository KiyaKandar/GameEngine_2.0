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
	unsigned int numberOfThreadSenders = ProcessScheduler::Retrieve()->GetTotalNumberOfThreads();

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

void MessageProcessor::AddDefaultMessageActions()
{
	actionsToExecute[DUMMY_WORK] = new std::vector<Action>();
	actionsToExecute.at(DUMMY_WORK)->push_back([](Message* message)
	{
		DummyWorkMessage* dummyWorkMessage = static_cast<DummyWorkMessage*>(message);
		dummyWorkMessage->DoDummyWork();
	});
}