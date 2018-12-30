#pragma once

#include "Menu.h"
#include "../Launch/Systems/Subsystem.h"
#include "../../Input/Devices/Keyboard.h"

class UserInterface : public Subsystem
{
public:
	UserInterface(Keyboard* mouse, NCLVector2 resolution);
	~UserInterface();

	void SetMenuFile(std::string newMenuFile);
	void Initialise(Database* database);

	void UpdateNextFrame(const float& deltaTime) override;

private:
	void ToggleModule();

	std::string menuFile;
	Keyboard* keyboard;
	NCLVector2 resolution;
	Menu* menu = nullptr;
	bool enabled = true;
	bool blocked;

	SinglePressKeyListener escapeListener;
	SinglePressKeyListener downListener;
	SinglePressKeyListener upListener;
	SinglePressKeyListener leftListener;
	SinglePressKeyListener returnListener;
};

