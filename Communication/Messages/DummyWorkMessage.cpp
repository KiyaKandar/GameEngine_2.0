#include "DummyWorkMessage.h"
#include <thread>
#include "Maths/Vector3.h"
#include "Communication/DeliverySystem.h"

DummyWorkMessage::DummyWorkMessage(const std::string& destination, const int workloadDurationMicroseconds, bool randomiseDestination)
	: Message(destination, DUMMY_WORK)
{
	this->workloadDurationMicroseconds = workloadDurationMicroseconds;
	this->randomiseDestination = randomiseDestination;
}

DummyWorkMessage::~DummyWorkMessage()
{
}

void DummyWorkMessage::RandomiseDestination()
{
	if (randomiseDestination)
	{
		const std::vector<std::string>& deliveryPoints = DeliverySystem::GetPostman()->GetAllDeliveryPoints();
		destination = deliveryPoints[rand() % (deliveryPoints.size() - 1)];
	}
}

void DummyWorkMessage::DoDummyWork() const
{
	if (workloadDurationMicroseconds == -1)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(rand() % 1000000 + 10000));
	}
	else
	{
		std::this_thread::sleep_for(std::chrono::microseconds(workloadDurationMicroseconds));
	}

	ForceMemoryAllocationsAndCacheMisses();
}
DummyWorkMessage DummyWorkMessage::Builder(Node* node)
{
	std::string nodeDestination = ""; //something random
	int workloadDurationMicroseconds = -1;
	bool randomiseDestination = true;

	for (Node* childNode : node->children)
	{
		if (childNode->nodeType == "destination")
		{
			randomiseDestination = false;
			nodeDestination = childNode->value;
		}
		if (childNode->nodeType == "duration")
		{
			const float durationMilliseconds = stof(childNode->value);
			workloadDurationMicroseconds = int(durationMilliseconds * 1000.0f);
		}
	}

	return DummyWorkMessage(nodeDestination, workloadDurationMicroseconds, randomiseDestination);
}

void DummyWorkMessage::ForceMemoryAllocationsAndCacheMisses() const
{
	const int numElements = 1000 * 1000;
	int* memoryBlock = (int*)malloc(numElements * sizeof(int));

	for (int i = 0; i < numElements / 10; i++) 
	{
		const int randomIndex = rand() % numElements;
		memoryBlock[randomIndex] = 0;
	}

	free(memoryBlock);
}
