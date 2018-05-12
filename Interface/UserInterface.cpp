#include "UserInterface.h"

#include "../../Input/Devices/Keyboard.h"
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
	if (keyboard->keyTriggered(KEYBOARD_ESCAPE))
	{
		toggleModule();
	}

	if (enabled && !blocked)
	{
		if (keyboard->keyTriggered(KEYBOARD_DOWN))
		{
			UserInterfaceDisplay::getInterface()->moveSelectedDown();
		}
		else if (keyboard->keyTriggered(KEYBOARD_UP))
		{
			UserInterfaceDisplay::getInterface()->moveSelectedUp();
		}
		else if (keyboard->keyTriggered(KEYBOARD_LEFT))
		{
			UserInterfaceDisplay::getInterface()->moveSelectedLeft();
		}

		if (keyboard->keyTriggered(KEYBOARD_RETURN))
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
