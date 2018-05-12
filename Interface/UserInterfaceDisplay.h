#pragma once
#include "UserInterfaceService.h"

class UserInterfaceDisplay
{
public:
	static UserInterfaceService* getInterface()
	{
		return service;
	}

	static void provide(UserInterfaceService* newService)
	{
		service = newService;
	}

private:
	static UserInterfaceService* service;
};

