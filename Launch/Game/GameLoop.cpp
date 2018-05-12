#include "GameLoop.h"
#include "../../Input/InputManager.h"


#include "../../Resource Management/XMLParser.h"
#include "../../Resource Management/Level.h"
#include "../../Input/Recorders/KeyboardMouseRecorder.h"
#include <iostream>
#include "Communication/LetterBox.h"
#include "../../Gameplay/GameObject.h"
#include "../../Input/Recorders/KeyboardMouseRecorder.h"
#include "Communication/Messages/PlaySoundMessage.h"
#include "../Startup.h"
#include <iterator>

GameLoop::GameLoop(System* gameSystem, Database* database, Startup* startup)
{
	engine = gameSystem;
	this->database = database;

	DeliverySystem::getPostman()->addDeliveryPoint("GameLoop");
	incomingMessages = MessageProcessor(std::vector<MessageType>{ MessageType::TEXT},
		DeliverySystem::getPostman()->getDeliveryPoint("GameLoop"));

	incomingMessages.addActionToExecuteOnMessage(MessageType::TEXT, [startup = startup, &quit = quit,
		&deltaTimeMultiplier = deltaTimeMultiplier, &engine = engine](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		istringstream iss(textMessage->text);
		vector<string> tokens{ istream_iterator<string>{iss},
			std::istream_iterator<string>{} };

		if (tokens[0] == "Quit")
		{
			quit = true;
		}
		else if (tokens[0] == "Start")
		{
			DeliverySystem::getPostman()->cancelOutgoingMessages();
			startup->switchLevel();
			deltaTimeMultiplier = 1.0f;

			if (tokens[1] == "True")
			{
				startup->beginOnlineLobby();
				startup->loadLevel(tokens[2], true);
			}
			else
			{
				engine->removeSubsystem("NetworkClient");
				startup->loadLevel(tokens[2], false);
			}

			startup->setupMeshes();
			startup->startUserInterface();

			XMLParser::deleteAllParsedXML();
		}
		else if (tokens[0] == "deltatime")
		{
			if (tokens[1] == "enable")
			{
				deltaTimeMultiplier = 1.0f;
			}
			else if (tokens[1] == "disable")
			{
				deltaTimeMultiplier = 0.0f;
			}
		}
	});
}

GameLoop::~GameLoop()
{
}

void GameLoop::executeGameLoop()
{
	camera->setPitch(24.0f);
	camera->setYaw(-133.0f);

	while (window->updateWindow() && !quit)
	{
		float deltaTime = loopTimer->getTimeSinceLastRetrieval() * deltaTimeMultiplier;

		engine->updateNextSystemFrame(deltaTime);
		incomingMessages.processMessagesInBuffer();

		DeliverySystem::getPostman()->clearAllMessages();
		updateGameObjects(deltaTime);
		DeliverySystem::getPostman()->deliverAllMessages();
	}
}

void GameLoop::updateGameObjects(float deltaTime)
{
	auto gameObjectResources = database->getTable("GameObjects")->getAllResources()->getResourceBuffer();

	for (auto gameObjectIterator = gameObjectResources.begin(); gameObjectIterator != gameObjectResources.end(); ++gameObjectIterator)
	{
		GameObject* gObj = static_cast<GameObject*>((*gameObjectIterator).second);

		if (gObj->getPhysicsNode() != nullptr)
		{
			gObj->update(deltaTime);
		}
	}
}
