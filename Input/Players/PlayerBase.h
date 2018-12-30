#pragma once
#include "Player.h"
#include "../Resource Management/XMLParser.h"
#include "../Input/InputUtility.h"
#include "../../Gameplay/InputGameplay/InputActionMap.h"
#include <map>
#include <vector>

class Database;

class PlayerBase
{
public:
	PlayerBase(Database* database);
	PlayerBase(Database* database, std::vector<InputRecorder*> allRecorders);
	~PlayerBase();

	void InitializePlayers(std::vector<InputRecorder*> allRecorders);

	Player* AddNewPlayer(InputRecorder* recorder, int id);
	void RemovePlayer(int playerID);

	std::vector<Player*>& GetPlayers()
	{
		return players;
	}

	std::vector<InputActionMap>& GetPlayersAction()
	{
		return playersActions;
	}

	void ClearPlayeractions()
	{
		playersActions.clear();
	}

	bool* b;

private:
	Player* GetExistingPlayer(Player* player, int existingID);
	void WipeStoredPlayers();

	std::vector<InputRecorder*> inputRecorders;
	std::vector<Player*> players;
	std::vector<InputActionMap> playersActions;

	XMLParser inputParser;
	Database* database;
};