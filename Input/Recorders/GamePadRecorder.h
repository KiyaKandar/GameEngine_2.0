#pragma once
#include "InputRecorder.h"
#include "../Devices/Gamepad.h"

class GamePadRecorder : public InputRecorder
{
public:
	explicit GamePadRecorder(Gamepad* gamePad);
	~GamePadRecorder();

	void FillInputs() override;

private:
	Gamepad* gamepad;
};

