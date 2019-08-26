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
		DeliverySystem::getPostman()->getDeliveryPoint("Gameplay"));

	incomingMessages.addActionToExecuteOnMessage(MessageType::TEXT, [this, database = database](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		istringstream iss(textMessage->text);
		vector<string> tokens{ istream_iterator<string>{iss},
			std::istream_iterator<string>{} };

		if (tokens[0] == "addgameobjectlogic")
		{
			objects.push_back(GameObjectLogic(database, tokens[1]));
			objects[objects.size() - 1].compileParsedXMLIntoScript();
		}
		else if (tokens[0] == "removegameobjectlogic")
		{
			gameObjectLogicRemoveBuffer.push_back(tokens[1]);
		}
		else if (tokens[0] == "setgameplaylogic")
		{
			compileGameplayScript(tokens[1]);
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
	
	incomingMessages.addActionToExecuteOnMessage(MessageType::PLAYER_INPUT, [&gameLogic = gameLogic, &inputBridge = inputBridge, &objects = objects](Message* message)
	{
		PlayerInputMessage* playerInputMessage = static_cast<PlayerInputMessage*>(message);
		
		inputBridge.processPlayerInputMessage(*playerInputMessage);


		for (GameObjectLogic& object : objects)
		{
   			object.notify("InputMessage", message, playerInputMessage->player->getGameObject()->getName());
		}
	});

	incomingMessages.addActionToExecuteOnMessage(MessageType::COLLISION, [&gameLogic = gameLogic, &objects = objects](Message* message)
	{
		CollisionMessage* collisionmessage = static_cast<CollisionMessage*>(message);

		gameLogic.notifyMessageActions("CollisionMessage", message);

		
		
		for (GameObjectLogic& object : objects)
		{
			object.notify("CollisionMessage", message, collisionmessage->objectIdentifier);
		}

	});

	timer->addChildTimer("Level Logic");
	timer->addChildTimer("Object Logic");
}

GameplaySystem::~GameplaySystem()
{
}

void GameplaySystem::updateNextFrame(const float& deltaTime)
{
	if (gameLogic.isTimed) 
	{
		updateGameplayWhenTimed(deltaTime);
	}
	else
	{
		timer->beginTimedSection();

		updateGameLogic(deltaTime);
		updateGameObjectLogics(deltaTime);

		timer->endTimedSection();
	}

	removeScriptsInbuffer();
}

void GameplaySystem::connectPlayerbase(PlayerBase* playerBase)
{
	inputBridge = GameplayInputBridge();

	for (size_t i = 0; i < playerBase->getPlayers().size(); i++)//every ionput action map in playersInGame
	{
		inputBridge.addInputActionMapForPlayer(playerBase->getPlayersAction()[i]);
	}
}

void GameplaySystem::compileGameplayScript(std::string levelScript)
{
	ActionBuilder::randomIntervals.clear();
	ActionBuilder::setExecutableBuilder([](Node* node)
	{
		return SendMessageActionBuilder::buildSendMessageAction(node);
	});

	gameplayScript = levelScript;
	gameLogic = GameLogic(&incomingMessages);
	gameLogic.compileScript(levelScript);
	gameLogic.executeActionsOnStart();
}

void GameplaySystem::setDefaultGameplayScript()
{
	gameplayScript = "";
	gameLogic = GameLogic(&incomingMessages);
}

void GameplaySystem::addGameObjectScript(std::string scriptFile)
{
	objects.push_back(GameObjectLogic(database, scriptFile));
}

void GameplaySystem::deleteGameObjectScripts()
{
	objects.clear();
}

void GameplaySystem::compileGameObjectScripts()
{
	for (GameObjectLogic& object : objects)
	{
		object.compileParsedXMLIntoScript();
	}
}

void GameplaySystem::updateGameplayWhenTimed(const float& deltaTime)
{
	if (gameLogic.elapsedTime < gameLogic.maxTime)
	{
		updateGameplayWithTimeRemaining(deltaTime);
	}
	else if (!levelFinished)
	{
		levelFinished = true;
		DeliverySystem::getPostman()->insertMessage(TextMessage("GameLoop", "deltatime disable"));
		DeliverySystem::getPostman()->insertMessage(TextMessage("UserInterface", "Toggle"));
	}
	else
	{
		updateGameOverScreen();
	}
}

void GameplaySystem::updateGameplayWithTimeRemaining(const float& deltaTime)
{
	timer->beginTimedSection();

	updateGameLogic(deltaTime);
	updateGameObjectLogics(deltaTime);

	timer->endTimedSection();

	updateGameTimer(deltaTime);
	PaintGameActionBuilder::updateBufferedVariables();
}

void GameplaySystem::updateGameOverScreen()
{
	DeliverySystem::getPostman()->insertMessage(TextMeshMessage("RenderingSystem", "GAME OVER!",
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

	DeliverySystem::getPostman()->insertMessage(TextMeshMessage("RenderingSystem", "Player" + std::to_string(winningPlayerID) + " wins!!! :)",
		NCLVector3(-50, -100, 0), NCLVector3(20, 20, 20), NCLVector3(1, 1, 1), true, true));
}

void GameplaySystem::updateGameLogic(const float& deltaTime)
{
	timer->beginChildTimedSection("Level Logic");
	gameLogic.executeMessageBasedActions();
	gameLogic.executeTimeBasedActions(deltaTime * 0.001f);
	gameLogic.clearNotifications();
	timer->endChildTimedSection("Level Logic");
}

void GameplaySystem::updateGameObjectLogics(const float& deltaTime)
{
	timer->beginChildTimedSection("Object Logic");
	for (GameObjectLogic& object : objects)
	{
		object.updatelogic(deltaTime * 0.001f);
	}
	timer->endChildTimedSection("Object Logic");
}

void GameplaySystem::removeScriptsInbuffer()
{
	for (std::string gameObjectLogicToRemove : gameObjectLogicRemoveBuffer)
	{
		for (size_t i = 0; i < objects.size(); ++i)
		{
			if (objects[i].getScriptFile() == gameObjectLogicToRemove)
			{
				objects.erase(objects.begin() + i);
				break;
			}
		}
	}

	gameObjectLogicRemoveBuffer.clear();
}

void GameplaySystem::updateGameTimer(const float& deltaTime)
{
	gameLogic.elapsedTime += (deltaTime * 0.001f);

	DeliverySystem::getPostman()->insertMessage(TextMeshMessage("RenderingSystem", std::to_string((int)round(gameLogic.maxTime - gameLogic.elapsedTime)),
		NCLVector3(-75, 310, 0), NCLVector3(30, 30, 30), NCLVector3(1, 0, 0), true, true));
}

