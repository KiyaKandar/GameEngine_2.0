#pragma once

#include "AnimationService.h"

class AnimationPlayer
{
public:
	static AnimationService* getAnimationService()
	{
		return service;
	}

	static void provide(AnimationService* newService)
	{
		service = newService;
	}

private:
	static AnimationService* service;
};

