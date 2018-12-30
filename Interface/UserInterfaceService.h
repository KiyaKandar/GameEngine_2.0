#pragma once

#include "../Graphics/Graphics Pipeline/PipelineConfiguration.h"

#include <vector>

class UserInterfaceService
{
public:
	virtual std::vector<Button*>* GetAllButtonsInMenu() = 0;
	virtual void MoveSelectedDown() = 0;
	virtual void MoveSelectedUp() = 0;
	virtual void MoveSelectedLeft() = 0;
	virtual void ExecuteSelectedButton() = 0;
};

