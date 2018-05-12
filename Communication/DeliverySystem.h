#pragma once
#include "MessagingService.h"

//service locator
class DeliverySystem
{
public:
	static MessagingService* getPostman() 
	{
		return service;
	}

	static void provide(MessagingService* newService)
	{
		service = newService;
	}

private:
	static MessagingService* service;
};