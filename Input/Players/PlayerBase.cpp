#include "PlayerBase.h"

#include "../Recorders/InputRecorder.h"
#include "../../Resource Management/Database/Database.h"
#include "../../Gameplay/GameObject.h"
#include "../../Physics/PhysicsNode.h"
#include "../../Communication/DeliverySystem.h"

PlayerBase::PlayerBase(Database* database)
{
	this->database = database;
}

PlayerBase::PlayerBase(Database* database, std::vector<InputRecorder*> allRecorders)
{
	this->database = database;
}

PlayerBase::~PlayerBase()
{
	WipeStoredPlayers();
}

void PlayerBase::InitializePlayers(std::vector<InputRecorder*> allRecorders)
{
	WipeStoredPlayers();

	int i = 0;
	for (InputRecorder* recorder : allRecorders)
	{
		AddNewPlayer(recorder, i);

		++i;
	}
}

Player* PlayerBase::AddNewPlayer(InputRecorder* recorder, int id)
{
	int playerID = id;

	for (Player* player : players)
	{
		if (player->getPlayerID() == playerID)
		{
			return GetExistingPlayer(player, id);
		}
	}

	Player* player = new Player(playerID, recorder);
	players.push_back(player);

	InputActionMap newPlayersActions(playerID);

	inputParser.LoadXmlFile("../Data/Input/configXML.xml");
	Node* node = inputParser.parsedXml;

	std::string playerName = "player" + std::to_string(id);
	MoveCameraRelativeToGameObjectMessage::resourceName = playerName;

	GameObject* playerGameObject = static_cast<GameObject*>(database->GetTable("GameObjects")->GetResource(playerName));
	player->setGameObject(playerGameObject);

	std::string seperator = "|";
	std::string keyboardButtonsToListenTo = "";

	for (size_t i = 0; i < inputParser.parsedXml->children.size(); i++) 
	{
		std::string keyName = inputParser.parsedXml->children[i]->children[0]->value;

		if (i != inputParser.parsedXml->children.size() - 1)
		{
			keyboardButtonsToListenTo += keyName + seperator;
		}
		else
		{
			keyboardButtonsToListenTo += keyName;
		}

		Node* magnitude = node->children[i]->children[1]->children[0];
		float xPosition = stof(magnitude->children[0]->value);
		float yPosition = stof(magnitude->children[1]->value);
		float zPosition = stof(magnitude->children[2]->value);

		NCLVector3 translation(xPosition, yPosition, zPosition);
		std::string type = magnitude->nodeType;

		newPlayersActions.AttachKeyToAction(InputUtility::GetKeyId(keyName), [translation, type, playerName](Player* player)
		{
			if (type == "Move")
			{
				ApplyForceMessage m("Physics", playerName, false, translation);
				DeliverySystem::GetPostman()->InsertMessage(m);
			}
			else if (type == "Impulse")
			{
				ApplyImpulseMessage m("Physics", playerName, false, translation);
				DeliverySystem::GetPostman()->InsertMessage(m);
			}
		});
	}

	std::vector<int> keyboardMouseConfiguration = player->getInputFilter()->getListenedKeys(keyboardButtonsToListenTo, seperator);
	player->getInputRecorder()->AddKeysToListen(keyboardMouseConfiguration);

	playersActions.push_back(newPlayersActions);
	return player;
}

void PlayerBase::RemovePlayer(int playerID)
{
	for (unsigned int i = 0; i < players.size(); ++i)
	{
		if (players[i]->getPlayerID() == playerID)
		{
			delete players[i];
			players.erase(players.begin() + i);
		}
	}
}

Player* PlayerBase::GetExistingPlayer(Player* player, int existingID)
{
	std::string playerName = "player" + std::to_string(existingID);
	GameObject* playerGameObject = static_cast<GameObject*>(database->GetTable("GameObjects")->GetResource(playerName));
	player->setGameObject(playerGameObject);

	return player;
}

void PlayerBase::WipeStoredPlayers()
{
	for (auto player : players)
	{
		delete player;
	}

	players.clear();
}
