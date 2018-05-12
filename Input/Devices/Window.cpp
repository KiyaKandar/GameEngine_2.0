#include "Window.h"
#include "Mouse.h"
#include "Keyboard.h"

Window* Window::window;

Keyboard*Window::keyboard = NULL;
Mouse*Window::mouse = NULL;

Window::Window(std::string title,
	int sizeX, int sizeY, bool fullScreen) {
	window = this;
	forceQuit = false;
	init = false;
	mouseLeftWindow = false;
	lockMouse = false;
	showMouse = true;

	this->fullScreen = fullScreen;

	size.x = (float)sizeX; size.y = (float)sizeY;

	fullScreen ? position.x = 0.0f : position.x = 100.0f;
	fullScreen ? position.y = 0.0f : position.y = 100.0f;

	HINSTANCE hInstance = GetModuleHandle(NULL);


	////This creates the console window
	//	AllocConsole();
	//
	//	int consoleHandle;	
	//	long stdHandle;
	//	FILE *file;	
	//
	//	// redirect stdout
	//	stdHandle		= (long)GetStdHandle(STD_OUTPUT_HANDLE);	
	//	consoleHandle	= _open_osfhandle(stdHandle, _O_TEXT);
	//	file	= _fdopen( consoleHandle, "w" );
	//	*stdout = *file;
	//	setvbuf( stdout, NULL, _IONBF, 0 );
	//
	//	// redirect stdin
	//	stdHandle	= (long)GetStdHandle(STD_INPUT_HANDLE);
	//	file		= _fdopen( consoleHandle, "r" );
	//	*stdin = *file;
	//	setvbuf( stdin, NULL, _IONBF, 0 );
	////
	//
	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	if (!GetClassInfoEx(hInstance, WINDOWCLASS, &windowClass)) {
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = (WNDPROC)WindowProc;
		windowClass.hInstance = hInstance;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
		windowClass.lpszClassName = WINDOWCLASS;

		if (!RegisterClassEx(&windowClass)) {
			return;
		}
	}

	if (fullScreen) {
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared

		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = sizeX;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight = sizeY;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel = 32;					// Selected Bits Per Pixel
		dmScreenSettings.dmDisplayFrequency = 60;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			return;
		}
	}

	windowHandle = CreateWindowEx(fullScreen ? WS_EX_TOPMOST : NULL,
		WINDOWCLASS,    // name of the window class
		title.c_str(),   // title of the window
		fullScreen ? WS_POPUP | WS_VISIBLE : WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,    // window style
		(int)position.x,	// x-position of the window
		(int)position.y,	// y-position of the window
		(int)size.x,		// width of the window
		(int)size.y,		// height of the window
		NULL,				// No parent window!
		NULL,				// No Menus!
		hInstance,			// application handle
		NULL);				// No multiple windows!

	if (!windowHandle) {
		return;
	}

	if (!keyboard) {
		keyboard = new Keyboard(windowHandle);
	}
	if (!mouse) {
		mouse = new Mouse(windowHandle);
	}

	timer = new GameTimer();
	elapsedMS = timer->getMillisecondsSinceStart();

	Window::getMouse()->setAbsolutePositionBounds((unsigned int)size.x, (unsigned int)size.y);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(window->windowHandle, &pt);
	Window::getMouse()->setAbsolutePosition(pt.x, pt.y);

	lockMouseToWindow(lockMouse);
	showOSPointer(showMouse);

	init = true;
}


Window::~Window(void)
{
	delete keyboard; keyboard = NULL;
	delete mouse;	mouse = NULL;

	//FreeConsole();		//Destroy the console window
}

HWND Window::getHandle() {
	return windowHandle;
}

bool Window::hasInitialised() {
	return init;
}

bool	Window::updateWindow() {
	MSG		msg;

	float diff = timer->getMillisecondsSinceStart() - elapsedMS;

	Window::getMouse()->updateDoubleClick(diff);

	Window::getKeyboard()->updateHolds();
	Window::getMouse()->updateHolds();

	while (PeekMessage(&msg, windowHandle, 0, 0, PM_REMOVE)) {
		checkMessages(msg);
	}

	elapsedMS = timer->getMillisecondsSinceStart();

	return !forceQuit;
}

void Window::checkMessages(MSG &msg) {
	switch (msg.message) {				// Is There A Message Waiting?
	case (WM_QUIT):
	case (WM_CLOSE): {					// Have We Received A Quit Message?
		window->showOSPointer(true);
		window->lockMouseToWindow(false);
		forceQuit = true;
	}break;
	case (WM_INPUT): {
		UINT dwSize;
		GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

		BYTE* lpb = new BYTE[dwSize];

		GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
		RAWINPUT* raw = (RAWINPUT*)lpb;

		if (keyboard && raw->header.dwType == RIM_TYPEKEYBOARD) {
			Window::getKeyboard()->update(raw);
		}

		if (mouse && raw->header.dwType == RIM_TYPEMOUSE) {
			Window::getMouse()->update(raw);
		}
		delete lpb;
	}break;

	default: {								// If Not, Deal With Window Messages
		TranslateMessage(&msg);				// Translate The Message
		DispatchMessage(&msg);				// Dispatch The Message
	}
	}
}

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case(WM_DESTROY): {
		window->showOSPointer(true);
		window->lockMouseToWindow(false);

		PostQuitMessage(0);
		window->forceQuit = true;
	} break;
	case (WM_ACTIVATE): {
		//int fMinimized	= (BOOL) HIWORD(wParam);
		if (LOWORD(wParam) == WA_INACTIVE) {
			ReleaseCapture();
			ClipCursor(NULL);
			mouse->sleep();
			keyboard->Sleep();
		}
		else {
			if (window->init) {
				mouse->wake();
				keyboard->wake();

				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(window->windowHandle, &pt);
				mouse->setAbsolutePosition(pt.x, pt.y);

				if (window->lockMouse) {
					window->lockMouseToWindow(true);
				}
			}
		}
		return 0;
	}break;
	case (WM_LBUTTONDOWN): {
		if (window->lockMouse) {
			window->lockMouseToWindow(true);
		}

	}break;

	case (WM_MOUSEMOVE): {
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = window->windowHandle;
		TrackMouseEvent(&tme);

		if (window->mouseLeftWindow) {
			window->mouseLeftWindow = false;
			mouse->wake();
			keyboard->wake();

			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(window->windowHandle, &pt);
			mouse->setAbsolutePosition(pt.x, pt.y);
		}

	}break;
	case(WM_MOUSELEAVE): {
		window->mouseLeftWindow = true;
		mouse->sleep();
		keyboard->Sleep();
	}break;
	case(WM_SIZE): {
		window->size.x = (float)LOWORD(lParam);
		window->size.y = (float)HIWORD(lParam);

		if (window->init) {
			mouse->setAbsolutePositionBounds(LOWORD(lParam), HIWORD(lParam));

			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(window->windowHandle, &pt);
			mouse->setAbsolutePosition(pt.x, pt.y);

			window->lockMouseToWindow(window->lockMouse);
		}
	}break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void	Window::lockMouseToWindow(bool lock) {
	lockMouse = lock;
	if (lock) {
		RECT		windowRect;
		GetWindowRect(window->windowHandle, &windowRect);

		SetCapture(window->windowHandle);
		ClipCursor(&windowRect);

		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(window->windowHandle, &pt);
		Window::getMouse()->setAbsolutePosition(pt.x, pt.y);
	}
	else {
		ReleaseCapture();
		ClipCursor(NULL);
	}
}

void	Window::showOSPointer(bool show) {
	if (show == showMouse) {
		return;	//ShowCursor does weird things, due to being a counter internally...
	}

	showMouse = show;
	if (show) {
		ShowCursor(1);
	}
	else {
		ShowCursor(0);
	}
}