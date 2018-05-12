#include "GameLogic.h"

#include "../Communication/MessageProcessor.h"
#include "../Resource Management/XMLParser.h"
#include "ActionBuilder.h"

GameLogic::GameLogic(MessageProcessor* messages)
{
	this->messages = messages;
}

GameLogic::GameLogic(MessageProcessor* messages, Node* xmlNode)
{
	this->messages = messages;
	compileParsedXMLIntoScript(xmlNode);
}

GameLogic::~GameLogic()
{
}

void GameLogic::compileScript(std::string scriptFile)
{
	XMLParser xmlParser;
	xmlParser.loadXMLFile(scriptFile);
	this->scriptFile = scriptFile;

	compileParsedXMLIntoScript(xmlParser.parsedXml);
}

void GameLogic::compileParsedXMLIntoScript(Node* xmlNode)
{
	for (Node* gameplayAction : xmlNode->children)
	{
		if (gameplayAction->nodeType == "ReceiveMessage")
		{
			for (Node* action : gameplayAction->children)
			{
				messageBasedActions[gameplayAction->name].push_back(ActionBuilder::buildAction(action));
			}
		}
		else if (gameplayAction->nodeType == "Timed")
		{
			timers.push_back(float(0.0f));
			timedActions.push_back(ActionBuilder::buildTimedAction(gameplayAction));
		}
		else if (gameplayAction->nodeType == "OnStart")
		{
			for (Node* action : gameplayAction->children)
			{
				actionsOnStart.push_back(ActionBuilder::compileActionSectionWithoutCondition(action));
			}
		}
	}
}

void GameLogic::executeMessageBasedActions()
{
	if (!messageBasedActions.empty())
	{
		for (size_t i = 0; i < publishers.size(); ++i)
		{
			if (publishers[i].first == "CollisionMessage" || publishers[i].first == "InputMessage")
			{
				for (GameplayAction& executable : messageBasedActions[publishers[i].first])
				{
					executable(publishers[i].second);
				}
			}
		}
	}
}

void GameLogic::executeTimeBasedActions(const float& deltaTime)
{
	for (size_t i = 0; i < timedActions.size(); ++i)
	{
		timers[i] += deltaTime;
		timedActions[i](timers[i]);
	}
}

void GameLogic::executeActionsOnStart()
{
	for (Executable executable : actionsOnStart)
	{
		executable();
	}
}

void GameLogic::notifyMessageActions(const std::string& messageType, Message* message)
{
	publishers.push_back(std::make_pair(messageType, *message));
}

void GameLogic::clearNotifications()
{
	publishers.clear();
}

std::string GameLogic::getScriptFile()
{
	return scriptFile;
}
