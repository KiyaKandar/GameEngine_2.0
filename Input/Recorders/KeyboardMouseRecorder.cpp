#include "KeyboardMouseRecorder.h"

#include <iostream>

KeyboardMouseRecorder::KeyboardMouseRecorder(Keyboard * keyboard, Mouse * mouse)
{
	this->keyboard = keyboard;
	this->mouse = mouse;
}

void KeyboardMouseRecorder::fillInputs()
{
	for (int key : keysToListen)
	{
		if (keyboard->keyHeld(static_cast<KeyboardKeys>(key))) 
		{
			currentButtonInputs.push_back(ButtonInputData(InputType::HELD, key));
		}
	}

	currentPositionalInputs.push_back(PositionalInputData(NCLVector2(),"relativePosition"));
	currentPositionalInputs.push_back(PositionalInputData(NCLVector2(),"absolutePosition"));
}

