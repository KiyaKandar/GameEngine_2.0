#pragma once
#include "UserInterfaceService.h"

class UserInterfaceDisplay
{
public:
	static UserInterfaceService* GetInterface()
	{
		return service;
	}

	static void Provide(UserInterfaceService* newService)
	{
		service = newService;
	}

private:
	static UserInterfaceService* service;
};

