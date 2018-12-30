#pragma once

#include "AnimationService.h"

class AnimationPlayer
{
public:
	static AnimationService* GetAnimationService()
	{
		return service;
	}

	static void Provide(AnimationService* newService)
	{
		service = newService;
	}

private:
	static AnimationService* service;
};

