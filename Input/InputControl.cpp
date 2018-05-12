#include "InputControl.h"
#include "Communication/DeliverySystem.h"

InputControl::InputControl()
{
}

InputControl::~InputControl()
{
}

void InputControl::registerNewInputUserByDeliveryPoint(std::string deliveryPoint)
{
	inputUserDeliveryPoints.push_back(deliveryPoint);
}

void InputControl::blockAllInputUsersOtherThanCaller(std::string caller)
{
	if (std::find(inputUserDeliveryPoints.begin(), inputUserDeliveryPoints.end(), 
		caller) != inputUserDeliveryPoints.end())
	{
		for (std::string deliveryPoint : inputUserDeliveryPoints)
		{
			if (deliveryPoint != caller)
			{
				blockedInputUsers.push_back(deliveryPoint);
				DeliverySystem::getPostman()->insertMessage(TextMessage(deliveryPoint, "BLOCK"));
			}
		}
	}
}

void InputControl::unlockBlockedUsers()
{
	for (std::string deliveryPoint : blockedInputUsers)
	{
		DeliverySystem::getPostman()->insertMessage(TextMessage(deliveryPoint, "UNBLOCK"));
	}

	blockedInputUsers.clear();
}

bool InputControl::isBlocked(const std::string revievedMessage)
{
	if (revievedMessage == "BLOCK")
	{
		return true;
	}
	
	if (revievedMessage == "UNBLOCK")
	{
		return false;
	}

	return false;
}
