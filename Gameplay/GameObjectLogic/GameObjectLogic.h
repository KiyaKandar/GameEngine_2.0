#pragma once

#include "../Communication/Message.h"
#include "../Gameplay/GameObject.h"
#include "../Gameplay/Scripting/ActionBuilder.h"
#include "../Resource Management/Database/Database.h"
#include "../Gameplay/Scripting/GameLogic.h"
#include "../../Communication/Messages/PlayerInputMessage.h"

class Database;

class GameObjectLogic
{
public:
	GameObjectLogic(Database* database, std::string script);
	~GameObjectLogic();

	void compileParsedXMLIntoScript();
	void notify(const std::string& messageType, Message* message, std::string gameObject = "");
	void updatelogic(const float& deltaTime);
	
	std::string getScriptFile()
	{
		return scriptFile;
	}

	std::vector<GameLogic> logics;

private:
	void compileGameLogic(Node* gameLogicNode, const std::vector<Node*>& resources);
	void compilePaintGameLogic(Node* paintGameNode, const std::vector<Node*>& resources);
	void compileLogicFromNodes(Node* logicNode, const std::vector<Node*>& resources);

	void changeResource(Node** node, std::string id);
	void changeResourceBack(Node** node, std::string id);

	std::string scriptFile;
	Node* parsedScript;
	Database* database;

	std::map<std::string, std::function<void()>> fucntionsOnStart;
};

