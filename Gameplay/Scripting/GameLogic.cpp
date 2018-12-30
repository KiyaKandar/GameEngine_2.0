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
	CompileParsedXmlIntoScript(xmlNode);
}

GameLogic::~GameLogic()
{
}

void GameLogic::CompileScript(std::string scriptFile)
{
	XMLParser xmlParser;
	xmlParser.LoadXmlFile(scriptFile);
	this->scriptFile = scriptFile;

	CompileParsedXmlIntoScript(xmlParser.parsedXml);
}

void GameLogic::CompileParsedXmlIntoScript(Node* xmlNode)
{
	for (Node* gameplayAction : xmlNode->children)
	{
		if (gameplayAction->nodeType == "ReceiveMessage")
		{
			for (Node* action : gameplayAction->children)
			{
				messageBasedActions[gameplayAction->name].push_back(ActionBuilder::BuildAction(action));
			}
		}
		else if (gameplayAction->nodeType == "Timed")
		{
			timers.push_back(float(0.0f));
			timedActions.push_back(ActionBuilder::BuildTimedAction(gameplayAction));
		}
		else if (gameplayAction->nodeType == "OnStart")
		{
			for (Node* action : gameplayAction->children)
			{
				actionsOnStart.push_back(ActionBuilder::CompileActionSectionWithoutCondition(action));
			}
		}
	}
}

void GameLogic::ExecuteMessageBasedActions()
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

void GameLogic::ExecuteTimeBasedActions(const float& deltaTime)
{
	for (size_t i = 0; i < timedActions.size(); ++i)
	{
		timers[i] += deltaTime;
		timedActions[i](timers[i]);
	}
}

void GameLogic::ExecuteActionsOnStart()
{
	for (Executable executable : actionsOnStart)
	{
		executable();
	}
}

void GameLogic::NotifyMessageActions(const std::string& messageType, Message* message)
{
	publishers.push_back(std::make_pair(messageType, *message));
}

void GameLogic::ClearNotifications()
{
	publishers.clear();
}

std::string GameLogic::GetScriptFile()
{
	return scriptFile;
}
