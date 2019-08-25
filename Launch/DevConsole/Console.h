#pragma once

#include "../Systems/Subsystem.h"
#include "../../Input/Devices/Keyboard.h"

class Mouse;
class Camera;

class Console : public Subsystem
{
public:
	Console(Keyboard* keyboard, Camera* camera, Mouse* mouse);
	~Console();

	void UpdateNextFrame(const float& deltaTime) override;

private:
	void ToggleConsoleEnabled();
	void RecordKeyPresses();

	void TraverseInputHistory();
	void DeleteLastCharacter();
	void ReadKeyboardInputs();
	void DisplayText();

	void MoveCamera();

	Keyboard* keyboard;
	Mouse* mouse;
	Camera* camera;

	std::string input;
	bool enabled;
	bool blocked;
	bool debugCameraEnabled = false;
	bool capslock = false;

	int frameCount = 0;
	int previousInputIndexOffset = 0;
	float pitch = 0.0f;
	float yaw = 0.0f;

	std::deque<std::string> previousInputs;
	std::unordered_map<int, std::string> keyMapping;

	SinglePressKeyListener f7Listener;
	SinglePressKeyListener returnListener;
	SinglePressKeyListener capitalListener;
	SinglePressKeyListener upListener;
	SinglePressKeyListener downListener;
	SinglePressKeyListener backListener;

	std::vector<SinglePressKeyListener> keyListeners;
};
