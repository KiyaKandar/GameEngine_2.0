#include "GamePadRecorder.h"
#include <iostream>

GamePadRecorder::GamePadRecorder(Gamepad * gamePad)
{
	this->gamepad = gamePad;
}

GamePadRecorder::~GamePadRecorder()
{
}

void GamePadRecorder::FillInputs()
{
	gamepad->Update();

	for (int key : keysToListen) 
	{
		if (gamepad->GetButtonPressed(key)) 
		{
			currentButtonInputs.push_back(ButtonInputData(InputType::TRIGGERED,key));
		}

		if (gamepad->GetButtonDown(key)) 
		{
			currentButtonInputs.push_back(ButtonInputData(InputType::HELD,key));
		}
	}

	currentLinearInputs.push_back(LinearInputData(gamepad->LeftTrigger(),"leftTrigger"));
	currentLinearInputs.push_back(LinearInputData(gamepad->RightTrigger(),"rightTrigger"));
	
	currentPositionalInputs.push_back(PositionalInputData(NCLVector2(gamepad->LeftStick_X(),gamepad->LeftStick_Y()),"leftStick"));
	currentPositionalInputs.push_back(PositionalInputData(NCLVector2(gamepad->RightStick_X(), gamepad->RightStick_Y()), "rightStick"));
}
