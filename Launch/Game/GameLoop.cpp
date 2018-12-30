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

	DeliverySystem::GetPostman()->AddDeliveryPoint("GameLoop");
	incomingMessages = MessageProcessor(std::vector<MessageType>{ MessageType::TEXT},
		DeliverySystem::GetPostman()->GetDeliveryPoint("GameLoop"));

	incomingMessages.AddActionToExecuteOnMessage(MessageType::TEXT, [startup = startup, &quit = quit,
		&deltaTimeMultiplier = deltaTimeMultiplier, &engine = engine](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		istringstream iss(textMessage->text);
		vector<string> tokens{ istream_iterator<string>{iss},
			std::istream_iterator<string>{} };

		if (tokens[0] == "Quit")
		{
			quit = true;
			engine->SynchroniseAndStopConcurrentSubsystems();
		}
		else if (tokens[0] == "Start")
		{
			engine->SynchroniseAndStopConcurrentSubsystems();
			DeliverySystem::GetPostman()->CancelOutgoingMessages();
			DeliverySystem::GetPostman()->CancelDeliveredMessages();
			DeliverySystem::GetPostman()->DeleteAllTrackedSenders();
			startup->SwitchLevel();
			deltaTimeMultiplier = 1.0f;

			if (tokens[1] == "True")
			{
				startup->BeginOnlineLobby();
				startup->LoadLevel(tokens[2], true);
			}
			else
			{
				engine->RemoveSubsystem("NetworkClient");
				startup->LoadLevel(tokens[2], false);
			}

			startup->SetupMeshes();
			startup->StartUserInterface();

			XMLParser::DeleteAllParsedXml();
			engine->StartConcurrentSubsystems();
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

void GameLoop::ExecuteGameLoop()
{
	camera->SetPitch(24.0f);
	camera->SetYaw(-133.0f);

	engine->StartConcurrentSubsystems();

	while (window->UpdateWindow() && !quit)
	{
		engine->UpdateNextSystemFrame();
		incomingMessages.ProcessMessagesInBuffer();
		
		UpdateGameObjects(loopTimer->GetTimeSinceLastRetrieval() * deltaTimeMultiplier);
	}
}

void GameLoop::UpdateGameObjects(float deltaTime) const
{
	auto gameObjectResources = database->GetTable("GameObjects")->GetAllResources()->GetResourceBuffer();

	for (auto gameObjectIterator = gameObjectResources.begin(); gameObjectIterator != gameObjectResources.end(); ++gameObjectIterator)
	{
		GameObject* gObj = static_cast<GameObject*>((*gameObjectIterator).second);

		if (gObj->GetPhysicsNode() != nullptr)
		{
			gObj->Update(deltaTime);
		}
	}
}
