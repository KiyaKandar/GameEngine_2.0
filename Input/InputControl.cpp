#include "InputControl.h"
#include "Communication/DeliverySystem.h"

InputControl::InputControl()
{
}

InputControl::~InputControl()
{
}

void InputControl::RegisterNewInputUserByDeliveryPoint(std::string deliveryPoint)
{
	inputUserDeliveryPoints.push_back(deliveryPoint);
}

void InputControl::BlockAllInputUsersOtherThanCaller(std::string caller)
{
	if (std::find(inputUserDeliveryPoints.begin(), inputUserDeliveryPoints.end(), 
		caller) != inputUserDeliveryPoints.end())
	{
		for (std::string deliveryPoint : inputUserDeliveryPoints)
		{
			if (deliveryPoint != caller)
			{
				blockedInputUsers.push_back(deliveryPoint);
				DeliverySystem::GetPostman()->InsertMessage(TextMessage(deliveryPoint, "BLOCK"));
			}
		}
	}
}

void InputControl::UnlockBlockedUsers()
{
	for (std::string deliveryPoint : blockedInputUsers)
	{
		DeliverySystem::GetPostman()->InsertMessage(TextMessage(deliveryPoint, "UNBLOCK"));
	}

	blockedInputUsers.clear();
}

bool InputControl::IsBlocked(const std::string revievedMessage)
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
