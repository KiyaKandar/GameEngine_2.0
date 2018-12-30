#pragma once
#include "MessagingService.h"

//service locator
class DeliverySystem
{
public:
	static MessagingService* GetPostman() 
	{
		return service;
	}

	static void Provide(MessagingService* newService)
	{
		service = newService;
	}

private:
	static MessagingService* service;
};