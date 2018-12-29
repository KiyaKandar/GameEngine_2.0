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

	incomingMessages = MessageProcessor(types, DeliverySystem::getPostman()->getDeliveryPoint("UserInterface"));
	DeliverySystem::getPostman()->insertMessage(TextMessage("InputManager", "RegisterInputUser UserInterface"));

	incomingMessages.addActionToExecuteOnMessage(MessageType::TEXT, [&blocked = blocked, this](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		if (textMessage->text == "Toggle")
		{
			this->toggleModule();
		}
		else
		{
			blocked = InputControl::isBlocked(textMessage->text);
		}
	});

	menu = nullptr;
	DeliverySystem::getPostman()->insertMessage(TextMessage("InputManager", "BlockAllInputs UserInterface"));

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

void UserInterface::setMenuFile(std::string newMenuFile)
{
	menuFile = newMenuFile;
}

void UserInterface::initialise(Database* database)
{
	if (menu != nullptr)
	{
		delete menu;
	}

	auto UIMeshesResources = database->getTable("UIMeshes")->getAllResources()->getResourceBuffer();
	for (auto UIMeshIterator = UIMeshesResources.begin(); UIMeshIterator != UIMeshesResources.end(); UIMeshIterator++)
	{
		static_cast<Mesh*>((*UIMeshIterator).second)->setupMesh();
	}

	menu = new Menu(menuFile, database);
	UserInterfaceDisplay::provide(menu);
}

void UserInterface::updateNextFrame(const float& deltaTime)
{
	if (escapeListener.keyPressed())
	{
		toggleModule();
	}

	if (enabled && !blocked)
	{
		if (downListener.keyPressed())
		{
			UserInterfaceDisplay::getInterface()->moveSelectedDown();
		}
		else if (upListener.keyPressed())
		{
			UserInterfaceDisplay::getInterface()->moveSelectedUp();
		}
		else if (leftListener.keyPressed())
		{
			UserInterfaceDisplay::getInterface()->moveSelectedLeft();
		}

		if (returnListener.keyPressed())
		{
			UserInterfaceDisplay::getInterface()->ExecuteSelectedButton();
		}
	}
}

void UserInterface::toggleModule()
{
	if (enabled)
	{
		enabled = false;
		DeliverySystem::getPostman()->insertMessage(TextMessage("InputManager", "UnblockAll"));
		DeliverySystem::getPostman()->insertMessage(TextMessage("GameLoop", "deltatime enable"));
	}
	else
	{
		enabled = true;
		DeliverySystem::getPostman()->insertMessage(TextMessage("InputManager", "BlockAllInputs UserInterface"));
		DeliverySystem::getPostman()->insertMessage(TextMessage("GameLoop", "deltatime disable"));
	}

	DeliverySystem::getPostman()->insertMessage(ToggleGraphicsModuleMessage("RenderingSystem", "UIModule", enabled));
}
