#pragma once

#include "../Systems/Subsystem.h"
#include "../../Input/Devices/Keyboard.h"
#include "../../Communication/MessageSenders/TrackedMessageSender.h"

class Mouse;
class Camera;

class Console : public Subsystem
{
public:
	Console(Keyboard* keyboard, Camera* camera, Mouse* mouse);
	~Console();

	void updateNextFrame(const float& deltaTime) override;

private:
	void toggleConsoleEnabled();
	void recordKeyPresses();

	void traverseInputHistory();
	void deleteLastCharacter();
	void readKeyboardInputs();
	void displayText();

	void moveCamera();

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

	TrackedMessageSender<TextMeshMessage> consoleViewMessage;

	SinglePressKeyListener f7Listener;
	SinglePressKeyListener returnListener;
	SinglePressKeyListener capitalListener;
	SinglePressKeyListener upListener;
	SinglePressKeyListener downListener;
	SinglePressKeyListener backListener;

	std::vector<SinglePressKeyListener> keyListeners;
};

