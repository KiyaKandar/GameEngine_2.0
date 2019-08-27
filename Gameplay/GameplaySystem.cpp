#include "GameplaySystem.h"

#include <iostream>
#include "../Communication/Messages/PlayerInputMessage.h"
#include "../Input/Players/Player.h"
#include "../Graphics/Scene Management/SceneNode.h"
#include "../Input/InputUtility.h"
#include "../Resource Management/XMLParser.h"
#include "../Utilities/GameTimer.h"
#include "../Input/Devices/Keyboard.h"
#include "../Communication/SendMessageActionBuilder.h"
#include "Scripting\PaintGameActionBuilder.h"

GameplaySystem::GameplaySystem(Database* database)
	: Subsystem("Gameplay")
{
	this->database = database;
 
	incomingMessages = MessageProcessor(std::vector<MessageType> { MessageType::PLAYER_INPUT, MessageType::COLLISION, MessageType::TEXT },
		DeliverySystem::GetPostman()->GetDeliveryPoint("Gameplay"));

	incomingMessages.AddActionToExecuteOnMessage(MessageType::TEXT, [this, database = database](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		istringstream iss(textMessage->text);
		vector<string> tokens{ istream_iterator<string>{iss},
			std::istream_iterator<string>{} };

		if (tokens[0] == "addgameobjectlogic")
		{
			objects.push_back(GameObjectLogic(database, tokens[1]));
			objects[objects.size() - 1].CompileParsedXmlIntoScript();
		}
		else if (tokens[0] == "removegameobjectlogic")
		{
			gameObjectLogicRemoveBuffer.push_back(tokens[1]);
		}
		else if (tokens[0] == "setgameplaylogic")
		{
			CompileGameplayScript(tokens[1]);
		}
		else if (tokens[0] == "setmaxtime")
		{
			if(tokens[1] == "true")
			{
				gameLogic.isTimed = true;
				gameLogic.maxTime = stof(tokens[2]);
			}
			else
			{
				gameLogic.isTimed = false;
			}
		}
		else if (tokens[0] == "sendscore")
		{
			int playerID = stoi(tokens[1]);
			int playerScore = stoi(tokens[2]);

			playerScores[playerID] = playerScore;
		}
		
	});
	
	incomingMessages.AddActionToExecuteOnMessage(MessageType::PLAYER_INPUT, [&gameLogic = gameLogic, &inputBridge = inputBridge, &objects = objects](Message* message)
	{
		PlayerInputMessage* playerInputMessage = static_cast<PlayerInputMessage*>(message);
		
		inputBridge.ProcessPlayerInputMessage(*playerInputMessage);


		for (GameObjectLogic& object : objects)
		{
   			object.Notify("InputMessage", message, playerInputMessage->player->getGameObject()->GetName());
		}
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::COLLISION, [&gameLogic = gameLogic, &objects = objects](Message* message)
	{
		CollisionMessage* collisionmessage = static_cast<CollisionMessage*>(message);

		gameLogic.NotifyMessageActions("CollisionMessage", message);

		
		
		for (GameObjectLogic& object : objects)
		{
			object.Notify("CollisionMessage", message, collisionmessage->objectIdentifier);
		}

	});

	timer->AddChildTimer("Level Logic");
	timer->AddChildTimer("Object Logic");
}

GameplaySystem::~GameplaySystem()
{
}

void GameplaySystem::UpdateNextFrame(const float& deltaTime)
{
	if (gameLogic.isTimed) 
	{
		UpdateGameplayWhenTimed(deltaTime);
	}
	else
	{
		timer->BeginTimedSection();

		UpdateGameLogic(deltaTime);
		UpdateGameObjectLogics(deltaTime);

		timer->EndTimedSection();
	}

	RemoveScriptsInbuffer();
}

void GameplaySystem::ConnectPlayerbase(PlayerBase* playerBase)
{
	inputBridge = GameplayInputBridge();

	for (size_t i = 0; i < playerBase->GetPlayers().size(); i++)//every ionput action map in playersInGame
	{
		inputBridge.AddInputActionMapForPlayer(playerBase->GetPlayersAction()[i]);
	}
}

void GameplaySystem::CompileGameplayScript(std::string levelScript)
{
	ActionBuilder::randomIntervals.clear();
	ActionBuilder::SetExecutableBuilder([](Node* node)
	{
		return SendMessageActionBuilder::BuildSendMessageAction(node);
	});

	gameplayScript = levelScript;
	gameLogic = GameLogic(&incomingMessages);
	gameLogic.CompileScript(levelScript);
	gameLogic.ExecuteActionsOnStart();
}

void GameplaySystem::SetDefaultGameplayScript()
{
	gameplayScript = "";
	gameLogic = GameLogic(&incomingMessages);
}

void GameplaySystem::AddGameObjectScript(std::string scriptFile)
{
	objects.push_back(GameObjectLogic(database, scriptFile));
}

void GameplaySystem::DeleteGameObjectScripts()
{
	objects.clear();
}

void GameplaySystem::CompileGameObjectScripts()
{
	for (GameObjectLogic& object : objects)
	{
		object.CompileParsedXmlIntoScript();
	}
}

void GameplaySystem::UpdateGameplayWhenTimed(const float& deltaTime)
{
	if (gameLogic.elapsedTime < gameLogic.maxTime)
	{
		UpdateGameplayWithTimeRemaining(deltaTime);
	}
	else if (!levelFinished)
	{
		levelFinished = true;
		DeliverySystem::GetPostman()->InsertMessage(TextMessage("GameLoop", "deltatime disable"));
		DeliverySystem::GetPostman()->InsertMessage(TextMessage("UserInterface", "Toggle"));
	}
	else
	{
		UpdateGameOverScreen();
	}
}

void GameplaySystem::UpdateGameplayWithTimeRemaining(const float& deltaTime)
{
	timer->BeginTimedSection();


	UpdateGameLogic(deltaTime);
	UpdateGameObjectLogics(deltaTime);
	timer->EndTimedSection();

	UpdateGameTimer(deltaTime);
	PaintGameActionBuilder::UpdateBufferedVariables();
}

void GameplaySystem::UpdateGameOverScreen()
{
	DeliverySystem::GetPostman()->InsertMessage(TextMeshMessage("RenderingSystem", "GAME OVER!",
		NCLVector3(-50, -50, 0), NCLVector3(50, 50, 50), NCLVector3(1, 0, 0), true, true));

	int winningPlayerID = -1;
	int minScore = 0;

	for (auto playerScoreIterator = playerScores.begin(); playerScoreIterator != playerScores.end(); ++playerScoreIterator)
	{
		if ((*playerScoreIterator).second >= minScore)
		{
			winningPlayerID = (*playerScoreIterator).first;
			minScore = (*playerScoreIterator).second;
		}
	}

	DeliverySystem::GetPostman()->InsertMessage(TextMeshMessage("RenderingSystem", "Player" + std::to_string(winningPlayerID) + " wins!!! :)",
		NCLVector3(-50, -100, 0), NCLVector3(20, 20, 20), NCLVector3(1, 1, 1), true, true));
}

void GameplaySystem::UpdateGameLogic(const float& deltaTime)
{
	timer->BeginChildTimedSection("Level Logic");
	gameLogic.ExecuteMessageBasedActions();
	gameLogic.ExecuteTimeBasedActions(deltaTime * 0.001f);
	gameLogic.ClearNotifications();
	timer->EndChildTimedSection("Level Logic");
}

void GameplaySystem::UpdateGameObjectLogics(const float& deltaTime)
{
	timer->BeginChildTimedSection("Object Logic");
	for (GameObjectLogic& object : objects)
	{
		object.Updatelogic(deltaTime * 0.001f);
	}
	timer->EndChildTimedSection("Object Logic");
}

void GameplaySystem::RemoveScriptsInbuffer()
{
	for (std::string gameObjectLogicToRemove : gameObjectLogicRemoveBuffer)
	{
		for (size_t i = 0; i < objects.size(); ++i)
		{
			if (objects[i].GetScriptFile() == gameObjectLogicToRemove)
			{
				objects.erase(objects.begin() + i);
				break;
			}
		}
	}

	gameObjectLogicRemoveBuffer.clear();
}

void GameplaySystem::UpdateGameTimer(const float& deltaTime)
{
	gameLogic.elapsedTime += (deltaTime * 0.001f);

	DeliverySystem::GetPostman()->InsertMessage(TextMeshMessage("RenderingSystem", std::to_string((int)round(gameLogic.maxTime - gameLogic.elapsedTime)),
		NCLVector3(-75, 310, 0), NCLVector3(30, 30, 30), NCLVector3(1, 0, 0), true, true));
}

