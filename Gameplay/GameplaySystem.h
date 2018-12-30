#pragma once

#include "../Launch/Systems/Subsystem.h"
#include "InputGameplay/GameplayInputBridge.h"
#include "Scripting/GameLogic.h"
#include "../Resource Management/XMLParser.h"
#include "GameObjectLogic/GameObjectLogic.h"
#include "../Resource Management/Database/Database.h"
#include "../Communication/MessageSenders/TrackedMessageSender.h"

class Database;

class GameplaySystem : public Subsystem
{
public:
	GameplaySystem(Database* database);
	~GameplaySystem();

	void UpdateNextFrame(const float& deltaTime) override;

	void ConnectPlayerbase(PlayerBase* playerbase);
	void CompileGameplayScript(std::string levelScript);
	void SetDefaultGameplayScript();

	void AddGameObjectScript(std::string scriptFile);
	void DeleteGameObjectScripts();
	void CompileGameObjectScripts();

	std::string getGameplayFile()
	{
		return gameplayScript;
	}

	std::vector<GameObjectLogic>* GetGameObjectLogics()
	{
		return &objects;
	}

private:
	void UpdateGameplayWhenTimed(const float& deltaTime);
	void UpdateGameplayWithTimeRemaining(const float& deltaTime);
	void UpdateGameOverScreen();

	void UpdateGameLogic(const float& deltaTime);
	void UpdateGameObjectLogics(const float& deltaTime);
	void RemoveScriptsInbuffer();

	void UpdateGameTimer(const float& deltaTime);

	GameLogic gameLogic;
	std::vector<GameObjectLogic> objects;
	GameplayInputBridge inputBridge;
	XMLParser inputParser;
	Database* database;

	std::vector<std::string> gameObjectLogicRemoveBuffer;
	std::string gameplayScript = "";
	bool levelFinished = false;

	std::map<int, int> playerScores;
	TrackedMessageSender<TextMeshMessage> gameOverMessageSender;
	TrackedMessageSender<TextMeshMessage> winningPlayerMessageSender;
	TrackedMessageSender<TextMeshMessage> timerMessageSender;
};

