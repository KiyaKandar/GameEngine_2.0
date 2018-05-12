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
	parser.loadXMLFile(script);
	parsedScript = parser.parsedXml;
}

GameObjectLogic::~GameObjectLogic()
{
}

void GameObjectLogic::compileParsedXMLIntoScript()
{
	Node* resources = parsedScript->children[RESOURCE_NODE];

	compileGameLogic(parsedScript->children[GAME_LOGIC_NODE], resources->children);
	compilePaintGameLogic(parsedScript->children[PAINT_GAME_LOGIC_NODE], resources->children);

	for (GameLogic& logic : logics)
	{
		logic.executeActionsOnStart();
	}
}

void GameObjectLogic::notify(const std::string& messageType, Message* message, std::string gameObject)
{
	CollisionMessage* collisionmessage = static_cast<CollisionMessage*>(message);

	for (GameLogic& logic : logics)
	{
		if (gameObject == logic.gameObject)
		{
			logic.notifyMessageActions(messageType, message);
		}
		
	}
}

void GameObjectLogic::updatelogic(const float& deltaTime)
{
	for (GameLogic& logic : logics)
	{
		logic.executeMessageBasedActions();
		logic.executeTimeBasedActions(deltaTime);
		logic.clearNotifications();
	}
}

void GameObjectLogic::compileGameLogic(Node* gameLogicNode, const std::vector<Node*>& resources)
{
	ActionBuilder::setExecutableBuilder([](Node* node)
	{
		return SendMessageActionBuilder::buildSendMessageAction(node);
	});

	compileLogicFromNodes(gameLogicNode, resources);
}

void GameObjectLogic::compilePaintGameLogic(Node* paintGameNode, const std::vector<Node*>& resources)
{
	ActionBuilder::setExecutableBuilder([](Node* node)
	{
		return PaintGameActionBuilder::buildExecutable(node);
	});

	compileLogicFromNodes(paintGameNode, resources);
}

void GameObjectLogic::compileLogicFromNodes(Node* logicNode, const std::vector<Node*>& resources)
{
	for (Node* resource : resources)
	{
		GameObject* gObj = static_cast<GameObject*>(database->getTable("GameObjects")->getResource(resource->value));

		changeResource(&logicNode, resource->value);

		logics.push_back(GameLogic());
		logics[logics.size() - 1].compileParsedXMLIntoScript(logicNode);
		logics[logics.size() - 1].gameObject = resource->value;

		changeResourceBack(&logicNode, resource->value);
	}
}

void GameObjectLogic::changeResource(Node** node, std::string id)
{
	if ((*node)->value == "var")
	{
		(*node)->value = id;
	}
	for (Node* child : (*node)->children)
	{
		changeResource(&child, id);
	}
}

void GameObjectLogic::changeResourceBack(Node** node, std::string id)
{
	if ((*node)->value == id)
	{
		(*node)->value = "var";
	}
	for (Node* child : (*node)->children)
	{
		changeResourceBack(&child, id);
	}
}
