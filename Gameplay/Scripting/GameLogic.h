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

	void compileScript(std::string scriptFile);
	void compileParsedXMLIntoScript(Node* xmlNode);

	void executeMessageBasedActions();
	void executeTimeBasedActions(const float& deltaTime);
	void executeActionsOnStart();

	void notifyMessageActions(const std::string& messageType, Message* message);
	void clearNotifications();

	std::unordered_map<std::string, std::vector<GameplayAction>> getMessageBasedActionsMap() { return messageBasedActions; }
	std::string getScriptFile();

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


