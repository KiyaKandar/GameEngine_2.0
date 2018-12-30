#include "Gamepad.h"

#pragma comment(lib, "Xinput.lib")

//Define the 'XButtonIDs' struct as 'XButtons'
XButtonIDs XButtons;

//'XButtonIDs' Default constructor
XButtonIDs::XButtonIDs()
{
	// These values are used to index the XINPUT_Buttons array,
	// accessing the matching XINPUT button value

	A = 0;
	B = 1;
	X = 2;
	Y = 3;

	DPadUp = 4;
	DPadDown = 5;
	DPadLeft = 6;
	DPadRight = 7;

	LShoulder = 8;
	RShoulder = 9;

	LThumbstick = 10;
	RThumbstick = 11;

	Start = 12;
	Back = 13;
}

Gamepad::Gamepad()
{
	//Iterate through all gamepad buttons
	for (int i = 0; i < ButtonCount; i++)
	{
		bPrev_ButtonStates[i] = false;
		bButtonStates[i] = false;
		bGamepad_ButtonsDown[i] = false;
	}
}

Gamepad::Gamepad(int index)
{
	//index or 1, 2, 3 etc matches to 0, 1, 2 etc
	gamepadIndex = index - 1;

	//Iterate through all gamepad buttons
	for (int i = 0; i < ButtonCount; i++)
	{
		bPrev_ButtonStates[i] = false;
		bButtonStates[i] = false;
		bGamepad_ButtonsDown[i] = false;
	}
}

Gamepad::~Gamepad()
{
}

//Update gamepad state
void Gamepad::Update()
{
	state = GetState(); //Obtain current gamepad state

	// Iterate through all gamepad buttons
	for (int i = 0; i < ButtonCount; i++)
	{
		//Set button state for current frame
		bButtonStates[i] = (state.Gamepad.wButtons &
			XINPUT_Buttons[i]) == XINPUT_Buttons[i];

		//Set 'DOWN' state for current frame
		bGamepad_ButtonsDown[i] = !bPrev_ButtonStates[i] &&
			bButtonStates[i];
	}
}

//Update button states for next frame
void Gamepad::RefreshState()
{
	memcpy(bPrev_ButtonStates, bButtonStates,
		sizeof(bPrev_ButtonStates));
}

//Deadzone check for Left Thumbstick
bool Gamepad::LStickInDeadzone() const
{
	//Obtain the X & Y axes of the stick
	const short sX = state.Gamepad.sThumbLX;
	const short sY = state.Gamepad.sThumbLY;

	//X axis is outside of deadzone
	if (sX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
		sX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		return false;
	}

	//Y axis is outside of deadzone
	if (sY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
		sY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		return false;
	}

	//One (or both axes) axis is inside of deadzone
	return true;
}

//Deadzone check for Right Thumbstick
bool Gamepad::RStickInDeadzone() const
{
	//Obtain the X & Y axes of the stick
	const short sX = state.Gamepad.sThumbRX;
	const short sY = state.Gamepad.sThumbRY;

	//X axis is outside of deadzone
	if (sX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
		sX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		return false;
	}

	//Y axis is outside of deadzone
	if (sY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
		sY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		return false;
	}

	//One (or both axes) axis is inside of deadzone
	return true;
}

float Gamepad::LeftStick_X() const
{
	//Obtain X axis of left stick
	const short sX = state.Gamepad.sThumbLX;

	//Return axis value, converted to a float
	return (static_cast<float>(sX) / 32768.0f);
}

float Gamepad::LeftStick_Y() const
{
	//Obtain Y axis of left stick
	const short sY = state.Gamepad.sThumbLY;

	//Return axis value, converted to a float
	return (static_cast<float>(sY) / 32768.0f);
}

float Gamepad::RightStick_X() const
{
	//Obtain X axis of right stick
	const short sX = state.Gamepad.sThumbRX;

	//Return axis value, converted to a float
	return (static_cast<float>(sX) / 32768.0f);
}

float Gamepad::RightStick_Y() const
{
	//Obtain the Y axis of the left stick
	short sY = state.Gamepad.sThumbRY;

	//Return axis value, converted to a float
	return (static_cast<float>(sY) / 32768.0f);
}

float Gamepad::LeftTrigger() const
{
	//Obtain value of left trigger
	const BYTE Trigger = state.Gamepad.bLeftTrigger;

	if (Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		return Trigger / 255.0f;
	}
	else
	{
		return 0.0f; //Trigger was not pressed
	}
}

float Gamepad::RightTrigger() const
{
	//Obtain value of right trigger
	const BYTE Trigger = state.Gamepad.bRightTrigger;

	if (Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		return Trigger / 255.0f;
	}
	else
	{
		return 0.0f; //Trigger was not pressed
	}
}

//Return true if button is pressed, false if not
bool Gamepad::GetButtonPressed(const int button) const
{
	if (state.Gamepad.wButtons & XINPUT_Buttons[button])
	{
		return true; //The button is pressed
	}

	return false; //The button is not pressed
}

//Frame-specific version of 'GetButtonPressed' function
bool Gamepad::GetButtonDown(const int button) const
{
	return bGamepad_ButtonsDown[button];
}

//Vibrate the gamepad (values of 0.0f to 1.0f only)
void Gamepad::Rumble(const float a_fLeftMotor, const float a_fRightMotor) const
{
	XINPUT_VIBRATION VibrationState;
	ZeroMemory(&VibrationState, sizeof(XINPUT_VIBRATION));

	//Calculate vibration values
	const int iLeftMotor = int(a_fLeftMotor * 65535.0f);
	const int iRightMotor = int(a_fRightMotor * 65535.0f);

	//Set vibration values
	VibrationState.wLeftMotorSpeed = iLeftMotor;
	VibrationState.wRightMotorSpeed = iRightMotor;

	//Set the vibration state
	XInputSetState(gamepadIndex, &VibrationState);
}

XINPUT_STATE Gamepad::GetState() const
{
	//Temporary state to return
	XINPUT_STATE gamepadState;

	ZeroMemory(&gamepadState, sizeof(XINPUT_STATE));

	XInputGetState(gamepadIndex, &gamepadState);

	return gamepadState;
}

//Return gamepad index
int Gamepad::GetIndex() const
{
	return gamepadIndex;
}

void Gamepad::SetIndex(const int index)
{
	gamepadIndex = index;
}

//Return true if the gamepad is connected
bool Gamepad::Connected()
{
	//Zero memory
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	//Get the state of the gamepad
	const DWORD Result = XInputGetState(gamepadIndex, &state);

	if (Result == ERROR_SUCCESS)
	{
		return true; //The gamepad is connected
	}
	else
	{
		return false; //The gamepad is not connected
	}
}
