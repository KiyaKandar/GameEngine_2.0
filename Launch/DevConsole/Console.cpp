#include "Console.h"

#include "../../Input/Devices/Keyboard.h"
#include "../../Input/InputControl.h"
#include "Communication/SendMessageActionBuilder.h"
#include "../Graphics/Utility/Camera.h"
#include "../Input/Devices/Mouse.h"
#include "LevelEditor.h"

int consoleKeys[] =
{
	KEYBOARD_0,
	KEYBOARD_1,
	KEYBOARD_2,
	KEYBOARD_3,
	KEYBOARD_4,
	KEYBOARD_5,
	KEYBOARD_6,
	KEYBOARD_7,
	KEYBOARD_8,
	KEYBOARD_9,
	KEYBOARD_Q,
	KEYBOARD_W,
	KEYBOARD_E,
	KEYBOARD_R,
	KEYBOARD_T,
	KEYBOARD_Y,
	KEYBOARD_U,
	KEYBOARD_I,
	KEYBOARD_O,
	KEYBOARD_P,
	KEYBOARD_A,
	KEYBOARD_S,
	KEYBOARD_D,
	KEYBOARD_F,
	KEYBOARD_G,
	KEYBOARD_H,
	KEYBOARD_J,
	KEYBOARD_K,
	KEYBOARD_L,
	KEYBOARD_Z,
	KEYBOARD_X,
	KEYBOARD_C,
	KEYBOARD_V,
	KEYBOARD_B,
	KEYBOARD_N,
	KEYBOARD_M,
	KEYBOARD_SPACE,
	KEYBOARD_COMMA,
	KEYBOARD_PLUS,
	KEYBOARD_MINUS,
	KEYBOARD_DIVIDE,
	KEYBOARD_PERIOD
};

Console::Console(Keyboard* keyboard, Camera* camera, Mouse* mouse) : Subsystem("Console")
{
	incomingMessages = MessageProcessor(std::vector<MessageType> {MessageType::TEXT},
		DeliverySystem::getPostman()->getDeliveryPoint("Console"));

	this->keyboard = keyboard;
	this->mouse = mouse;
	this->camera = camera;
	enabled = false;
	blocked = false;

	DeliverySystem::getPostman()->insertMessage(TextMessage("InputManager", "RegisterInputUser Console"));

	incomingMessages.addActionToExecuteOnMessage(MessageType::TEXT, [&blocked = blocked, &debugCameraEnabled = debugCameraEnabled](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		if (textMessage->text == "togglecamera")
		{
			debugCameraEnabled = !debugCameraEnabled;

			if (debugCameraEnabled)
			{
				DeliverySystem::getPostman()->insertMessage(TextMessage("InputManager", "BlockAllInputs Console"));
			}
			else
			{
				DeliverySystem::getPostman()->insertMessage(TextMessage("InputManager", "UnblockAll"));
			}
		}
		else
		{
			blocked = InputControl::isBlocked(textMessage->text);
		}
	});

	keyMapping.insert({ KEYBOARD_0, "0" });
	keyMapping.insert({ KEYBOARD_1, "1" });
	keyMapping.insert({ KEYBOARD_2, "2" });
	keyMapping.insert({ KEYBOARD_3, "3" });
	keyMapping.insert({ KEYBOARD_4, "4" });
	keyMapping.insert({ KEYBOARD_5, "5" });
	keyMapping.insert({ KEYBOARD_6, "6" });
	keyMapping.insert({ KEYBOARD_7, "7" });
	keyMapping.insert({ KEYBOARD_8, "8" });
	keyMapping.insert({ KEYBOARD_9, "9" });
	keyMapping.insert({ KEYBOARD_Q, "q" });
	keyMapping.insert({ KEYBOARD_W, "w" });
	keyMapping.insert({ KEYBOARD_E, "e" });
	keyMapping.insert({ KEYBOARD_R, "r" });
	keyMapping.insert({ KEYBOARD_T, "t" });
	keyMapping.insert({ KEYBOARD_Y, "y" });
	keyMapping.insert({ KEYBOARD_U, "u" });
	keyMapping.insert({ KEYBOARD_I, "i" });
	keyMapping.insert({ KEYBOARD_O, "o" });
	keyMapping.insert({ KEYBOARD_P, "p" });
	keyMapping.insert({ KEYBOARD_A, "a" });
	keyMapping.insert({ KEYBOARD_S, "s" });
	keyMapping.insert({ KEYBOARD_D, "d" });
	keyMapping.insert({ KEYBOARD_F, "f" });
	keyMapping.insert({ KEYBOARD_G, "g" });
	keyMapping.insert({ KEYBOARD_H, "h" });
	keyMapping.insert({ KEYBOARD_J, "j" });
	keyMapping.insert({ KEYBOARD_K, "k" });
	keyMapping.insert({ KEYBOARD_L, "l" });
	keyMapping.insert({ KEYBOARD_Z, "z" });
	keyMapping.insert({ KEYBOARD_X, "x" });
	keyMapping.insert({ KEYBOARD_C, "c" });
	keyMapping.insert({ KEYBOARD_V, "v" });
	keyMapping.insert({ KEYBOARD_B, "b" });
	keyMapping.insert({ KEYBOARD_N, "n" });
	keyMapping.insert({ KEYBOARD_M, "m" });
	keyMapping.insert({ KEYBOARD_SPACE, " " });
	keyMapping.insert({ KEYBOARD_COMMA, "," });
	keyMapping.insert({ KEYBOARD_PLUS, "=" });
	keyMapping.insert({ KEYBOARD_MINUS, "-" });
	keyMapping.insert({ KEYBOARD_DIVIDE, "/" });
	keyMapping.insert({ KEYBOARD_PERIOD, "." });
}

Console::~Console()
{
}

void Console::updateNextFrame(const float & deltaTime)
{
	if (debugCameraEnabled)
	{
		moveCamera();
	}

	if (keyboard->keyTriggered(KEYBOARD_F7))
	{
		toggleConsoleEnabled();
	}

	if (enabled && !blocked)
	{
		recordKeyPresses();

		if (keyboard->keyTriggered(KEYBOARD_RETURN))
		{
			try
			{
				previousInputs.push_front(input);
				LevelEditor::executeDevConsoleLine(input);
				input = "";
			}
			catch(...)
			{
				input = "error";
			}
		}
	}
}

void Console::toggleConsoleEnabled()
{
	if (!debugCameraEnabled)
	{
		if (enabled)
		{
			DeliverySystem::getPostman()->insertMessage(TextMessage("InputManager", "UnblockAll"));
			enabled = false;
		}
		else
		{
			DeliverySystem::getPostman()->insertMessage(TextMessage("InputManager", "BlockAllInputs Console"));
			enabled = true;
		}
	}
}

void Console::recordKeyPresses()
{
	++frameCount;

	traverseInputHistory();
	deleteLastCharacter();

	if(keyboard->keyTriggered(KEYBOARD_CAPITAL))
	{
		capslock =  !capslock;
	}

	readKeyboardInputs();
	displayText();
}

void Console::traverseInputHistory()
{

	if (keyboard->keyTriggered(KEYBOARD_UP))
	{
		if ((size_t)previousInputIndexOffset < previousInputs.size())
		{
			input = previousInputs[previousInputIndexOffset];
			++previousInputIndexOffset;
		}
	}
	else if (keyboard->keyTriggered(KEYBOARD_DOWN))
	{
		if (previousInputIndexOffset > 0 && previousInputs.size() > 0)
		{
			--previousInputIndexOffset;
			input = previousInputs[previousInputIndexOffset];
		}
	}
}

void Console::deleteLastCharacter()
{

	if (keyboard->keyDown(KEYBOARD_BACK) && frameCount >= 5)
	{
		frameCount = 0;

		if (input.size() > 0)
		{
			input.pop_back();
		}
	}
}

void Console::readKeyboardInputs()
{

	for (int key : consoleKeys)
	{
		if (keyboard->keyStates[key] && !(keyboard->keyStates[key] && keyboard->holdStates[key]))
		{
			if (capslock)
			{
				std::string str = keyMapping.at(key);
				std::transform(str.begin(), str.end(), str.begin(), ::toupper);
				input += str;
			}
			else
			{
				input += keyMapping.at(key);
			}
		}
	}
}

void Console::displayText()
{
	std::string displayLine = input;

	for (size_t i = 0; i < 100 - input.size(); ++i)
	{
		displayLine += " ";
	}

	DeliverySystem::getPostman()->insertMessage(TextMeshMessage("RenderingSystem", displayLine,
		NCLVector3(-620.0f, -320, 0), NCLVector3(12.9f, 12.9f, 12.9f), NCLVector3(0, 1, 0), true, true));
}

void Console::moveCamera()
{
	pitch -= (mouse->getRelativePosition().y);
	yaw -= (mouse->getRelativePosition().x);

	if (keyboard->keyDown(KEYBOARD_W)) {
		camera->setPosition(camera->getPosition() +
			NCLMatrix4::rotation(yaw, NCLVector3(0, 1, 0)) * NCLVector3(0, 0, -1) * 1);
	}

	if (keyboard->keyDown(KEYBOARD_S)) {
		camera->setPosition(camera->getPosition() +
			NCLMatrix4::rotation(yaw, NCLVector3(0, 1, 0)) * NCLVector3(0, 0, 1) * 1);
	}

	if (keyboard->keyDown(KEYBOARD_A)) {
		camera->setPosition(camera->getPosition() +
			NCLMatrix4::rotation(yaw, NCLVector3(0, 1, 0)) *  NCLVector3(-1, 0, 0) * 1);
	}

	if (keyboard->keyDown(KEYBOARD_D)) {
		camera->setPosition(camera->getPosition() +
			NCLMatrix4::rotation(yaw, NCLVector3(0, 1, 0)) *  NCLVector3(1, 0, 0) * 1);
	}

	if (keyboard->keyDown(KEYBOARD_SPACE)) {
		camera->setPosition(camera->getPosition() + NCLVector3(0, 1, 0) * 1);
	}

	if (keyboard->keyDown(KEYBOARD_C)) {
		camera->setPosition(camera->getPosition() + NCLVector3(0, -1, 0) * 1);
	}

	camera->setPitch(pitch);
	camera->setYaw(yaw);
}
