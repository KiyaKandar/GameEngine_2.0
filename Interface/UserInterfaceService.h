#pragma once

#include "../Graphics/Graphics Pipeline/PipelineConfiguration.h"

#include <vector>

class UserInterfaceService
{
public:
	virtual std::vector<Button*>* getAllButtonsInMenu() = 0;
	virtual void moveSelectedDown() = 0;
	virtual void moveSelectedUp() = 0;
	virtual void moveSelectedLeft() = 0;
	virtual void ExecuteSelectedButton() = 0;
};

