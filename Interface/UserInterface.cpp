#include "UserInterface.h"

#include "UserInterfaceDisplay.h"
#include "../Input/InputControl.h"
#include "../Resource Management/Database/Database.h"

UserInterface::UserInterface(Keyboard* keyboard, NCLVector2 resolution) : Subsystem("UserInterface")
{
	this->keyboard = keyboard;
	this->resolution = resolution;
	blocked = false;

	std::vector<MessageType> types = { MessageType::TEXT };

	incomingMessages = MessageProcessor(types, DeliverySystem::GetPostman()->GetDeliveryPoint("UserInterface"));
	DeliverySystem::GetPostman()->InsertMessage(TextMessage("InputManager", "RegisterInputUser UserInterface"));

	incomingMessages.AddActionToExecuteOnMessage(MessageType::TEXT, [&blocked = blocked, this](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		if (textMessage->text == "Toggle")
		{
			this->ToggleModule();
		}
		else
		{
			blocked = InputControl::IsBlocked(textMessage->text);
		}
	});

	menu = nullptr;
	DeliverySystem::GetPostman()->InsertMessage(TextMessage("InputManager", "BlockAllInputs UserInterface"));

	escapeListener = SinglePressKeyListener(KEYBOARD_ESCAPE, keyboard);
	downListener = SinglePressKeyListener(KEYBOARD_DOWN, keyboard);
	upListener = SinglePressKeyListener(KEYBOARD_UP, keyboard);
	leftListener = SinglePressKeyListener(KEYBOARD_LEFT, keyboard);
	returnListener = SinglePressKeyListener(KEYBOARD_RETURN, keyboard);
}

UserInterface::~UserInterface()
{
	delete menu;
}

void UserInterface::SetMenuFile(std::string newMenuFile)
{
	menuFile = newMenuFile;
}

void UserInterface::Initialise(Database* database)
{
	if (menu != nullptr)
	{
		delete menu;
	}

	auto UIMeshesResources = database->GetTable("UIMeshes")->GetAllResources()->GetResourceBuffer();
	for (auto UIMeshIterator = UIMeshesResources.begin(); UIMeshIterator != UIMeshesResources.end(); UIMeshIterator++)
	{
		static_cast<Mesh*>((*UIMeshIterator).second)->SetupMesh();
	}

	menu = new Menu(menuFile, database);
	UserInterfaceDisplay::Provide(menu);
}

void UserInterface::UpdateNextFrame(const float& deltaTime)
{
	if (escapeListener.KeyPressed())
	{
		ToggleModule();
	}

	if (enabled && !blocked)
	{
		if (downListener.KeyPressed())
		{
			UserInterfaceDisplay::GetInterface()->MoveSelectedDown();
		}
		else if (upListener.KeyPressed())
		{
			UserInterfaceDisplay::GetInterface()->MoveSelectedUp();
		}
		else if (leftListener.KeyPressed())
		{
			UserInterfaceDisplay::GetInterface()->MoveSelectedLeft();
		}

		if (returnListener.KeyPressed())
		{
			UserInterfaceDisplay::GetInterface()->ExecuteSelectedButton();
		}
	}
}

void UserInterface::ToggleModule()
{
	if (enabled)
	{
		enabled = false;
		DeliverySystem::GetPostman()->InsertMessage(TextMessage("InputManager", "UnblockAll"));
		DeliverySystem::GetPostman()->InsertMessage(TextMessage("GameLoop", "deltatime enable"));
	}
	else
	{
		enabled = true;
		DeliverySystem::GetPostman()->InsertMessage(TextMessage("InputManager", "BlockAllInputs UserInterface"));
		DeliverySystem::GetPostman()->InsertMessage(TextMessage("GameLoop", "deltatime disable"));
	}

	DeliverySystem::GetPostman()->InsertMessage(ToggleGraphicsModuleMessage("RenderingSystem", "UIModule", enabled));
}
