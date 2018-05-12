#pragma once

#include "InputDevice.h"
#include "../../Utilities/Maths/Vector2.h"

//Presumably RAW input does actually support those fancy mice with greater
//than 5 buttons in some capacity, but I have a 5 button mouse so I don't
//care to find out how ;)
enum MouseButtons {
	MOUSE_LEFT = 0,
	MOUSE_RIGHT = 1,
	MOUSE_MIDDLE = 2,
	MOUSE_FOUR = 3,
	MOUSE_FIVE = 4,
	MOUSE_MAX = 5
};

class Mouse : public InputDevice {
public:
	friend class Window;

	//Is this mouse button currently pressed down?
	bool	buttonDown(MouseButtons button);
	//Has this mouse button been held down for multiple frames?
	bool	buttonHeld(MouseButtons button);
	//Has this mouse button been double clicked?
	bool	doubleClicked(MouseButtons button);

	//Get how much this mouse has moved since last frame
	NCLVector2	getRelativePosition();
	//Get the window position of the mouse pointer
	NCLVector2 getAbsolutePosition();

	//Determines the maximum amount of ms that can pass between
	//2 mouse presses while still counting as a 'double click'
	void	setDoubleClickLimit(float msec);

	//Has the mouse wheel moved since the last update?
	bool	wheelMoved();
	//Get the mousewheel movement. Positive means scroll up,
	//negative means scroll down, 0 means no movement.
	int		getWheelMovement();

	//Sets the mouse sensitivity. Currently only affects the 'relative'
	//(i.e FPS-style) mouse movement. Students! Maybe you'd like to
	//implement a 'MenuSensitivity' for absolute movement?
	void	setMouseSensitivity(float amount);

	NCLVector2 getAbsoluteBounds() { return absolutePositionBounds; };

protected:
	Mouse(HWND &hwnd);
	~Mouse(void) {}

	//Internal function that updates the mouse variables from a 
	//raw input 'packet'
	virtual void	update(RAWINPUT* raw);
	//Updates the holdButtons array. Call once per frame!
	virtual void	updateHolds();
	//Sends the mouse to sleep (i.e window has been alt-tabbed away etc)
	virtual void	sleep();

	//Updates the doubleclicks array. Call once per frame!
	void			updateDoubleClick(float msec);

	//Set the mouse's current screen position. Maybe should be public?
	void			setAbsolutePosition(unsigned int x, unsigned int y);

	//Set the absolute screen bounds (<0 is always assumed dissallowed). Used
	//by the window resize routine...
	void			setAbsolutePositionBounds(unsigned int maxX, unsigned int maxY);

	//Current mouse absolute position
	NCLVector2		absolutePosition;
	//Current mouse absolute position maximum bounds
	NCLVector2		absolutePositionBounds;
	//How much as the mouse moved since the last raw packet?
	NCLVector2		relativePosition;
	//Current button down state for each button
	bool		buttons[MOUSE_MAX];
	//Current button held state for each button
	bool		holdButtons[MOUSE_MAX];
	//Current doubleClick counter for each button
	bool		doubleClicks[MOUSE_MAX];
	//Counter to remember when last mouse click occured
	float		lastClickTime[MOUSE_MAX];

	//last mousewheel updated position
	int			lastWheel;

	//Current mousewheel updated position
	int			frameWheel;

	//Max amount of ms between clicks count as a 'double click'
	float		clickLimit;

	//Mouse pointer sensitivity. Set this negative to get a headache!
	float		sensitivity;
};