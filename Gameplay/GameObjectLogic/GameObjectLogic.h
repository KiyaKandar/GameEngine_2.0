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

	void CompileParsedXmlIntoScript();
	void Notify(const std::string& messageType, Message* message, std::string gameObject = "");
	void Updatelogic(const float& deltaTime);
	
	std::string GetScriptFile()
	{
		return scriptFile;
	}

	std::vector<GameLogic> logics;

private:
	void CompileGameLogic(Node* gameLogicNode, const std::vector<Node*>& resources);
	void CompilePaintGameLogic(Node* paintGameNode, const std::vector<Node*>& resources);
	void CompileLogicFromNodes(Node* logicNode, const std::vector<Node*>& resources);

	void ChangeResource(Node** node, std::string id);
	void ChangeResourceBack(Node** node, std::string id);

	std::string scriptFile;
	Node* parsedScript;
	Database* database;

	std::map<std::string, std::function<void()>> fucntionsOnStart;
};

