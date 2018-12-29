#include "Keyboard.h"
Keyboard::Keyboard(HWND &hwnd) {
	//Initialise the arrays to false!
	ZeroMemory(keyStates, KEYBOARD_MAX * sizeof(bool));
	ZeroMemory(holdStates, KEYBOARD_MAX * sizeof(bool));
	ZeroMemory(releasedStates, KEYBOARD_MAX * sizeof(bool));

	//Tedious windows RAW input stuff
	rid.usUsagePage = HID_USAGE_PAGE_GENERIC;		//The keyboard isn't anything fancy
	rid.usUsage = HID_USAGE_GENERIC_KEYBOARD;	//but it's definitely a keyboard!
	rid.dwFlags = RIDEV_INPUTSINK;				//Yes, we want to always receive RAW input...
	rid.hwndTarget = hwnd;							//Windows OS window handle
	RegisterRawInputDevices(&rid, 1, sizeof(rid));		//We just want one keyboard, please!

	initialiseMapper();
}

/*
Updates variables controlling whether a keyboard key has been
held for multiple frames.
*/
void Keyboard::updateHolds() {
	memcpy(holdStates, keyStates, KEYBOARD_MAX * sizeof(bool));
}

/*
Sends the keyboard to sleep, so it doesn't process any
keypresses until it receives a Wake()
*/
void Keyboard::Sleep() {
	isAwake = false;	//Night night!
						//Prevents incorrectly thinking keys have been held / pressed when waking back up
	ZeroMemory(keyStates, KEYBOARD_MAX * sizeof(bool));
	ZeroMemory(holdStates, KEYBOARD_MAX * sizeof(bool));
	ZeroMemory(releasedStates, KEYBOARD_MAX * sizeof(bool));
}

/*
Returns if the key is down. Doesn't need bounds checking -
a KeyboardKeys enum is always in range
*/
bool Keyboard::keyDown(KeyboardKeys key) {
	return keyStates[key];
}

/*
Returns if the key is down, and has been held down for multiple updates.
Doesn't need bounds checking - a KeyboardKeys enum is always in range
*/
bool Keyboard::keyHeld(KeyboardKeys key) {
	if (keyDown(key) && holdStates[key]) {
		return true;
	}
	return false;
}

/*
Returns true only if the key is down, but WASN't down last update.
Doesn't need bounds checking - a KeyboardKeys enum is always in range
*/
bool Keyboard::keyTriggered(KeyboardKeys key) {
	return (keyDown(key) && !keyHeld(key));
}

bool Keyboard::keyReleased(KeyboardKeys key)
{
	return releasedStates[key];
}

/*
Updates the keyboard state with data received from the OS.
*/
void Keyboard::update(RAWINPUT* raw) {
	if (isAwake) {
		DWORD key = (DWORD)raw->data.keyboard.VKey;

		//We should do bounds checking!
		if (key < 0 || key > KEYBOARD_MAX) {
			return;
		}

		//First bit of the flags tag determines whether the key is down or up
		bool down = !(raw->data.keyboard.Flags & RI_KEY_BREAK);
		releasedStates[key] = keyStates[key] && !down;
		keyStates[key] = down;
	}
}
