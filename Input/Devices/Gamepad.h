#pragma once

#include <windows.h>
#include <Xinput.h>
#include <string>

// XInput Button values
static const WORD XINPUT_Buttons[] = {
	XINPUT_GAMEPAD_A,
	XINPUT_GAMEPAD_B,
	XINPUT_GAMEPAD_X,
	XINPUT_GAMEPAD_Y,
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_RIGHT,
	XINPUT_GAMEPAD_LEFT_SHOULDER,
	XINPUT_GAMEPAD_RIGHT_SHOULDER,
	XINPUT_GAMEPAD_LEFT_THUMB,
	XINPUT_GAMEPAD_RIGHT_THUMB,
	XINPUT_GAMEPAD_START,
	XINPUT_GAMEPAD_BACK
};

// XInput Button IDs
struct XButtonIDs
{
	// Function prototypes
	//---------------------//

	XButtonIDs(); // Default constructor

	// Member variables
	//---------------------//

	int A, B, X, Y; // 'Action' buttons

	// Directional Pad (D-Pad)
	int DPadUp, DPadDown, DPadLeft, DPadRight;

	// Shoulder ('Bumper') buttons
	int LShoulder, RShoulder;

	// Thumbstick buttons
	int LThumbstick, RThumbstick;

	int Start; // 'START' button
	int Back; // 'BACK' button
};

class Gamepad
{
public:
	Gamepad();
	Gamepad(int index);
	~Gamepad();

	void Update();
	void RefreshState(); //Update button states for next frame

	//Thumbsticks
	//Return true if stick is in deadzone
	bool LStickInDeadzone() const;
	bool RStickInDeadzone() const;

	//Return axis values
	float LeftStick_X() const;
	float LeftStick_Y() const;
	float RightStick_X() const;
	float RightStick_Y() const;

	// Trigger functions - Return value of trigger
	float LeftTrigger() const;
	float RightTrigger() const;

	//Button functions
	//- 'Pressed' - Return true if pressed, false if not
	bool GetButtonPressed(const int button) const;
	bool GetButtonDown(const int button) const;

	//Utility functions
	XINPUT_STATE GetState() const;
	int GetIndex() const;
	void SetIndex(const int index);
	bool Connected();

	// Vibrate the gamepad (0.0f none, 1.0f max speed)
	void Rumble(const float a_fLeftMotor = 0.0f, const float a_fRightMotor = 0.0f) const;

	int gamepadIndex; //eg 1, 2, 3, 4
private:
	XINPUT_STATE state; //Current gamepad state

	static const int ButtonCount = 14; //Total gamepad buttons
	bool bPrev_ButtonStates[ButtonCount]; //Previous frame button states
	bool bButtonStates[ButtonCount]; //Current frame button states

	bool bGamepad_ButtonsDown[ButtonCount]; //Buttons pressed on current frame
};

// Externally define the 'XButtonIDs' struct as 'XButtons'
extern XButtonIDs XButtons;
