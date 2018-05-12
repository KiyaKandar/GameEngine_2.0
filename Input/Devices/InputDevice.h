#pragma once

#include<windows.h>

/*
Microsoft helpfully don't seem to have this in any of their header files,
despite it being how RAW input works....GG guys.
*/
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC			((USHORT) 0x01)
#endif

#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE			((USHORT) 0x02)
#endif

#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD		((USHORT) 0x06)
#endif

class InputDevice {
protected:
	friend class Window;
	InputDevice(void) { isAwake = true; };
	~InputDevice(void) {};

protected:
	virtual void update(RAWINPUT* raw) = 0;

	virtual void updateHolds() {}
	virtual void sleep() { isAwake = false; }
	virtual void wake() { isAwake = true; }

	bool			isAwake;		//Is the device awake...
	RAWINPUTDEVICE	rid;			//Windows OS hook 
};

