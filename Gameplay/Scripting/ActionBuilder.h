#pragma once

#include <functional>
#include <vector>

class Message;
struct Node;

typedef std::function<void(Message)> GameplayAction;
typedef std::function<void(float&)> TimedGameplayAction;
typedef std::function<bool(Message)> Condition;
typedef std::function<void()> Executable;

class ActionBuilder
{
public:
	static GameplayAction buildAction(Node* node);
	static TimedGameplayAction buildTimedAction(Node* node);

	static void compileActionSection(Node* section, std::vector<Condition>& conditions, std::vector<Executable>& executables);
	static Executable compileActionSectionWithoutCondition(Node* section);

	static void setExecutableBuilder(std::function<Executable(Node*)> executableBuilder);
	static std::vector<float> randomIntervals;

private:
	static GameplayAction buildFinalActionWithCondition(std::vector<Condition>& conditions, std::vector<Executable>& executables);
	static GameplayAction buildFinalAction(std::vector<Executable>& executables);

	static Condition buildIfStatement(Node* node);

	static float generateRandomInterval(const float minInterval, const float maxInterval);
	static TimedGameplayAction buildTimedActionWithStaticInterval(const float interval, const std::vector<Executable>& executables);
	static TimedGameplayAction buildTimedActionWithRandomisedInterval(const int quantity, const float minInterval, const float maxInterval,
		const std::vector<Executable>& executables);

	static std::function<Executable(Node*)> executableBuilder;
};

