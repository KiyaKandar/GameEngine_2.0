#pragma once

#include <string>
#include <vector>

class TextMessage;

class InputControl
{
public:
	InputControl();
	~InputControl();

	void registerNewInputUserByDeliveryPoint(std::string deliveryPoint);
	void blockAllInputUsersOtherThanCaller(std::string caller);
	void unlockBlockedUsers();

	static bool isBlocked(const std::string revievedMessage);

private:
	std::vector<std::string> inputUserDeliveryPoints;
	std::vector<std::string> blockedInputUsers;
};

