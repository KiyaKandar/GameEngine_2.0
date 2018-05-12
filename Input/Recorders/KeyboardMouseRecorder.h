#pragma once
#include "InputRecorder.h"
#include "../Devices\Keyboard.h"
#include "../Devices\Mouse.h"


class KeyboardMouseRecorder :
	public InputRecorder
{
public:
	KeyboardMouseRecorder(Keyboard* keyboard, Mouse* mouse);
	~KeyboardMouseRecorder() {}

	 void fillInputs() override;

	 void setKeyboard(Keyboard* keyboard)
	 {
		 this->keyboard = keyboard;
	 }

private:
	Keyboard* keyboard;
	Mouse* mouse;
};

