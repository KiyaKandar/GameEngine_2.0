#pragma once

#include "ActionBuilder.h"

#include <string>
#include <vector>
#include <unordered_map>

struct Node;
class MessageProcessor;

class GameLogic
{
public:
	GameLogic() = default;
	explicit GameLogic(MessageProcessor* messages);
	GameLogic(MessageProcessor* messages, Node* xmlNode);
	~GameLogic();

	void CompileScript(std::string scriptFile);
	void CompileParsedXmlIntoScript(Node* xmlNode);

	void ExecuteMessageBasedActions();
	void ExecuteTimeBasedActions(const float& deltaTime);
	void ExecuteActionsOnStart();

	void NotifyMessageActions(const std::string& messageType, Message* message);
	void ClearNotifications();

	std::unordered_map<std::string, std::vector<GameplayAction>> GetMessageBasedActionsMap() { return messageBasedActions; }
	std::string GetScriptFile();

	float maxTime = 0.0f;
	float elapsedTime = 0.0f;

	bool isTimed = false;

	std::string gameObject = "";

private:
	MessageProcessor* messages;
	std::vector<std::pair<std::string, Message>> publishers;
	std::unordered_map<std::string, std::vector<GameplayAction>> messageBasedActions;
	std::vector<TimedGameplayAction> timedActions;
	std::vector<Executable> actionsOnStart;

	std::string scriptFile = "";
	std::vector<float> timers;

	
	

};


