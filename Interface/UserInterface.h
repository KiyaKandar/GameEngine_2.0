#pragma once

#include "Menu.h"
#include "../Launch/Systems/Subsystem.h"
#include "../../Input/Devices/Keyboard.h"

class UserInterface : public Subsystem
{
public:
	UserInterface(Keyboard* mouse, NCLVector2 resolution);
	~UserInterface();

	void setMenuFile(std::string newMenuFile);
	void initialise(Database* database);

	void updateNextFrame(const float& deltaTime) override;

private:
	void toggleModule();

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

