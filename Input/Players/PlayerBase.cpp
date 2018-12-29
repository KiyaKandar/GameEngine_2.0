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
	wipeStoredPlayers();
}

void PlayerBase::initializePlayers(std::vector<InputRecorder*> allRecorders)
{
	wipeStoredPlayers();

	int i = 0;
	for (InputRecorder* recorder : allRecorders)
	{
		addNewPlayer(recorder, i);

		++i;
	}
}

Player* PlayerBase::addNewPlayer(InputRecorder* recorder, int id)
{
	int playerID = id;

	for (Player* player : players)
	{
		if (player->getPlayerID() == playerID)
		{
			return getExistingPlayer(player, id);
		}
	}

	Player* player = new Player(playerID, recorder);
	players.push_back(player);

	InputActionMap newPlayersActions(playerID);

	inputParser.loadXMLFile("../Data/Input/configXML.xml");
	Node* node = inputParser.parsedXml;

	std::string playerName = "player" + std::to_string(id);
	MoveCameraRelativeToGameObjectMessage::resourceName = playerName;

	GameObject* playerGameObject = static_cast<GameObject*>(database->getTable("GameObjects")->getResource(playerName));
	player->setGameObject(playerGameObject);

	std::string seperator = "|";
	std::string keyboardButtonsToListenTo = "";

	b = new bool[inputParser.parsedXml->children.size()];

	for (size_t i = 0; i < inputParser.parsedXml->children.size(); i++)
	{
		b[i] = true;
	}

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

		newPlayersActions.attachKeyToAction(InputUtility::getKeyID(keyName), [&b = b[i], translation, type, playerName](Player* player)
		{
			if (b)
			{
				b = false;

				if (type == "Move")
				{
					ApplyForceMessage m("Physics", playerName, false, translation);
					m.senderAvailable = &b;
					DeliverySystem::getPostman()->insertMessage(m);
				}
				else if (type == "Impulse")
				{
					ApplyImpulseMessage m("Physics", playerName, false, translation);
					m.senderAvailable = &b;
					DeliverySystem::getPostman()->insertMessage(m);
				}
			}
		});
	}

	std::vector<int> keyboardMouseConfiguration = player->getInputFilter()->getListenedKeys(keyboardButtonsToListenTo, seperator);
	player->getInputRecorder()->addKeysToListen(keyboardMouseConfiguration);

	playersActions.push_back(newPlayersActions);
	return player;
}

void PlayerBase::removePlayer(int playerID)
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

Player* PlayerBase::getExistingPlayer(Player* player, int existingID)
{
	std::string playerName = "player" + std::to_string(existingID);
	GameObject* playerGameObject = static_cast<GameObject*>(database->getTable("GameObjects")->getResource(playerName));
	player->setGameObject(playerGameObject);

	return player;
}

void PlayerBase::wipeStoredPlayers()
{
	for (auto player : players)
	{
		delete player;
	}

	players.clear();
}
