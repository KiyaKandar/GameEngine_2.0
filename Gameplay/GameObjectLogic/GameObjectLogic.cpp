#include "GameObjectLogic.h"
#include "Devices/Keyboard.h"
#include "../../Communication/DeliverySystem.h"
#include "../../Communication/SendMessageActionBuilder.h"
#include "../Scripting/PaintGameActionBuilder.h"

const int RESOURCE_NODE = 0;
const int GAME_LOGIC_NODE = 1;
const int PAINT_GAME_LOGIC_NODE = 2;

GameObjectLogic::GameObjectLogic(Database* database, std::string script)
{
	this->database = database;

	scriptFile = script;
	XMLParser parser;
	parser.LoadXmlFile(script);
	parsedScript = parser.parsedXml;
}

GameObjectLogic::~GameObjectLogic()
{
}

void GameObjectLogic::CompileParsedXmlIntoScript()
{
	Node* resources = parsedScript->children[RESOURCE_NODE];

	CompileGameLogic(parsedScript->children[GAME_LOGIC_NODE], resources->children);
	CompilePaintGameLogic(parsedScript->children[PAINT_GAME_LOGIC_NODE], resources->children);

	for (GameLogic& logic : logics)
	{
		logic.ExecuteActionsOnStart();
	}
}

void GameObjectLogic::Notify(const std::string& messageType, Message* message, std::string gameObject)
{
	CollisionMessage* collisionmessage = static_cast<CollisionMessage*>(message);

	for (GameLogic& logic : logics)
	{
		if (gameObject == logic.gameObject)
		{
			logic.NotifyMessageActions(messageType, message);
		}
		
	}
}

void GameObjectLogic::Updatelogic(const float& deltaTime)
{
	for (GameLogic& logic : logics)
	{
		logic.ExecuteMessageBasedActions();
		logic.ExecuteTimeBasedActions(deltaTime);
		logic.ClearNotifications();
	}
}

void GameObjectLogic::CompileGameLogic(Node* gameLogicNode, const std::vector<Node*>& resources)
{
	ActionBuilder::SetExecutableBuilder([](Node* node)
	{
		return SendMessageActionBuilder::BuildSendMessageAction(node);
	});

	CompileLogicFromNodes(gameLogicNode, resources);
}

void GameObjectLogic::CompilePaintGameLogic(Node* paintGameNode, const std::vector<Node*>& resources)
{
	ActionBuilder::SetExecutableBuilder([](Node* node)
	{
		return PaintGameActionBuilder::BuildExecutable(node);
	});

	CompileLogicFromNodes(paintGameNode, resources);
}

void GameObjectLogic::CompileLogicFromNodes(Node* logicNode, const std::vector<Node*>& resources)
{
	for (Node* resource : resources)
	{
		GameObject* gObj = static_cast<GameObject*>(database->GetTable("GameObjects")->GetResource(resource->value));

		ChangeResource(&logicNode, resource->value);

		logics.push_back(GameLogic());
		logics[logics.size() - 1].CompileParsedXmlIntoScript(logicNode);
		logics[logics.size() - 1].gameObject = resource->value;

		ChangeResourceBack(&logicNode, resource->value);
	}
}

void GameObjectLogic::ChangeResource(Node** node, std::string id)
{
	if ((*node)->value == "var")
	{
		(*node)->value = id;
	}
	for (Node* child : (*node)->children)
	{
		ChangeResource(&child, id);
	}
}

void GameObjectLogic::ChangeResourceBack(Node** node, std::string id)
{
	if ((*node)->value == id)
	{
		(*node)->value = "var";
	}
	for (Node* child : (*node)->children)
	{
		ChangeResourceBack(&child, id);
	}
}
