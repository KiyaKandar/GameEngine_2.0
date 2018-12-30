#pragma once

#include <string>
#include <vector>

class TextMessage;

class InputControl
{
public:
	InputControl();
	~InputControl();

	void RegisterNewInputUserByDeliveryPoint(std::string deliveryPoint);
	void BlockAllInputUsersOtherThanCaller(std::string caller);
	void UnlockBlockedUsers();

	static bool IsBlocked(const std::string revievedMessage);

private:
	std::vector<std::string> inputUserDeliveryPoints;
	std::vector<std::string> blockedInputUsers;
};

