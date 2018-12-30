#include "Console.h"

#include "../../Input/InputControl.h"
#include "Communication/SendMessageActionBuilder.h"
#include "../Graphics/Utility/Camera.h"
#include "../Input/Devices/Mouse.h"
#include "LevelEditor.h"

KeyboardKeys consoleKeys[] =
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
	incomingMessages = MessageProcessor(std::vector<MessageType>{MessageType::TEXT},
		DeliverySystem::GetPostman()->GetDeliveryPoint("Console"));

	this->keyboard = keyboard;
	this->mouse = mouse;
	this->camera = camera;
	enabled = false;
	blocked = false;

	DeliverySystem::GetPostman()->InsertMessage(TextMessage("InputManager", "RegisterInputUser Console"));

	incomingMessages.AddActionToExecuteOnMessage(MessageType::TEXT,
		[&blocked = blocked, &debugCameraEnabled = debugCameraEnabled, mouse =
		this->mouse](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		if (textMessage->text == "togglecamera")
		{
			debugCameraEnabled = !debugCameraEnabled;

			if (debugCameraEnabled)
			{
				DeliverySystem::GetPostman()->InsertMessage(
					TextMessage("InputManager", "BlockAllInputs Console"));
				mouse->SetMouseSensitivity(0.00001f);
			}
			else
			{
				DeliverySystem::GetPostman()->InsertMessage(
					TextMessage("InputManager", "UnblockAll"));
				mouse->SetMouseSensitivity(0.07f);
			}
		}
		else
		{
			blocked = InputControl::IsBlocked(textMessage->text);
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

	f7Listener = SinglePressKeyListener(KEYBOARD_F7, keyboard);
	returnListener = SinglePressKeyListener(KEYBOARD_RETURN, keyboard);
	capitalListener = SinglePressKeyListener(KEYBOARD_CAPITAL, keyboard);
	upListener = SinglePressKeyListener(KEYBOARD_UP, keyboard);
	downListener = SinglePressKeyListener(KEYBOARD_DOWN, keyboard);
	backListener = SinglePressKeyListener(KEYBOARD_BACK, keyboard);

	for (int i = 0; i < 41; ++i)
	{
		keyListeners.push_back(SinglePressKeyListener((KeyboardKeys)consoleKeys[i], keyboard));
	}
}

Console::~Console()
{
}

void Console::UpdateNextFrame(const float& deltaTime)
{
	if (debugCameraEnabled)
	{
		MoveCamera();
	}

	if (f7Listener.KeyPressed())
	{
		ToggleConsoleEnabled();
	}

	if (enabled && !blocked)
	{
		RecordKeyPresses();

		if (returnListener.KeyPressed())
		{
			try
			{
				previousInputs.push_front(input);
				LevelEditor::ExecuteDevConsoleLine(input);
				input = "";
			}
			catch (...)
			{
				input = "error";
			}
		}
	}
}

void Console::ToggleConsoleEnabled()
{
	if (!debugCameraEnabled)
	{
		if (enabled)
		{
			DeliverySystem::GetPostman()->InsertMessage(TextMessage("InputManager", "UnblockAll"));
			enabled = false;
		}
		else
		{
			DeliverySystem::GetPostman()->InsertMessage(TextMessage("InputManager", "BlockAllInputs Console"));
			enabled = true;
		}
	}
}

void Console::RecordKeyPresses()
{
	++frameCount;

	TraverseInputHistory();
	DeleteLastCharacter();

	if (capitalListener.KeyPressed())
	{
		capslock = !capslock;
	}

	ReadKeyboardInputs();
	DisplayText();
}

void Console::TraverseInputHistory()
{
	if (upListener.KeyPressed())
	{
		if ((size_t)previousInputIndexOffset < previousInputs.size())
		{
			input = previousInputs[previousInputIndexOffset];
			++previousInputIndexOffset;
		}
	}
	else if (downListener.KeyPressed())
	{
		if (previousInputIndexOffset > 0 && previousInputs.size() > 0)
		{
			--previousInputIndexOffset;
			input = previousInputs[previousInputIndexOffset];
		}
	}
}

void Console::DeleteLastCharacter()
{
	if (backListener.KeyPressed() && frameCount >= 5)
	{
		frameCount = 0;

		if (input.size() > 0)
		{
			input.pop_back();
		}
	}
}

void Console::ReadKeyboardInputs()
{
	for (int i = 0; i < keyListeners.size(); ++i)
	{
		if (keyListeners[i].KeyPressed())
		{
			if (capslock)
			{
				std::string str = keyMapping.at(consoleKeys[i]);
				std::transform(str.begin(), str.end(), str.begin(), ::toupper);
				input += str;
			}
			else
			{
				input += keyMapping.at(consoleKeys[i]);
			}
		}
	}
}

void Console::DisplayText()
{
	std::string displayLine = input;

	for (size_t i = 0; i < 100 - input.size(); ++i)
	{
		displayLine += " ";
	}

	if (consoleViewMessage.ReadyToSendNextMessage())
	{
		consoleViewMessage.SetMessage(TextMeshMessage("RenderingSystem", displayLine,
			NCLVector3(-620.0f, -320, 0), NCLVector3(12.9f, 12.9f, 12.9f),
			NCLVector3(0, 1, 0), true, true));
		consoleViewMessage.SendTrackedMessage();
	}
}

void Console::MoveCamera()
{
	pitch -= (mouse->GetRelativePosition().y);
	yaw -= (mouse->GetRelativePosition().x);

	if (keyboard->KeyDown(KEYBOARD_W))
	{
		camera->SetPosition(camera->GetPosition() +
			NCLMatrix4::rotation(yaw, NCLVector3(0, 1, 0)) * NCLVector3(0, 0, -1) * 0.001f);
	}

	if (keyboard->KeyDown(KEYBOARD_S))
	{
		camera->SetPosition(camera->GetPosition() +
			NCLMatrix4::rotation(yaw, NCLVector3(0, 1, 0)) * NCLVector3(0, 0, 1) * 0.001f);
	}

	if (keyboard->KeyDown(KEYBOARD_A))
	{
		camera->SetPosition(camera->GetPosition() +
			NCLMatrix4::rotation(yaw, NCLVector3(0, 1, 0)) * NCLVector3(-1, 0, 0) * 0.001f);
	}

	if (keyboard->KeyDown(KEYBOARD_D))
	{
		camera->SetPosition(camera->GetPosition() +
			NCLMatrix4::rotation(yaw, NCLVector3(0, 1, 0)) * NCLVector3(1, 0, 0) * 0.001f);
	}

	if (keyboard->KeyDown(KEYBOARD_SPACE))
	{
		camera->SetPosition(camera->GetPosition() + NCLVector3(0, 1, 0) * 0.001f);
	}

	if (keyboard->KeyDown(KEYBOARD_C))
	{
		camera->SetPosition(camera->GetPosition() + NCLVector3(0, -1, 0) * 0.001f);
	}

	camera->SetPitch(pitch);
	camera->SetYaw(yaw);
}
