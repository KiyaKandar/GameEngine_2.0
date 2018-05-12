#pragma once
#pragma warning( disable : 4099 )

#include <string>

#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <unordered_map>

#include "Keyboard.h"
#include "Mouse.h"
#include "../../Utilities/GameTimer.h"

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define WINDOWCLASS "WindowClass"

class Window {
public:
	Window(std::string title = "OpenGL Framework",
		int sizeX = 800, int sizeY = 600, bool fullScreen = false);
	~Window(void);

	bool	updateWindow();

	HWND	getHandle();

	bool	hasInitialised();

	void	lockMouseToWindow(bool lock);
	void	showOSPointer(bool show);

	NCLVector2	getScreenSize() { return size; };

	static Keyboard*	getKeyboard() { return keyboard; }
	static Mouse*		getMouse() { return mouse; }


protected:
	void	checkMessages(MSG &msg);
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND			windowHandle;

	static Window*		window;
	static Keyboard*	keyboard;
	static Mouse*		mouse;

	bool				forceQuit;
	bool				init;
	bool				fullScreen;
	bool				lockMouse;
	bool				showMouse;

	NCLVector2				position;
	NCLVector2				size;

	float elapsedMS;
	GameTimer*	timer;

	bool				mouseLeftWindow;
};