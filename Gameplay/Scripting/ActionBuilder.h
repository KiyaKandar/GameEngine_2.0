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
	static GameplayAction BuildAction(Node* node);
	static TimedGameplayAction BuildTimedAction(Node* node);

	static void CompileActionSection(Node* section, std::vector<Condition>& conditions, std::vector<Executable>& executables);
	static Executable CompileActionSectionWithoutCondition(Node* section);

	static std::vector<float> randomIntervals;
	static void SetExecutableBuilder(std::function<Executable(Node*)> executableBuilder);
private:
	static GameplayAction BuildFinalActionWithCondition(std::vector<Condition>& conditions, std::vector<Executable>& executables);
	static GameplayAction BuildFinalAction(std::vector<Executable>& executables);

	static Condition BuildIfStatement(Node* node);

	static float generateRandomInterval(const float minInterval, const float maxInterval);
	static TimedGameplayAction buildTimedActionWithStaticInterval(const float interval, const std::vector<Executable>& executables);
	static TimedGameplayAction buildTimedActionWithRandomisedInterval(const int quantity, const float minInterval, const float maxInterval,
		const std::vector<Executable>& executables);

	static std::function<Executable(Node*)> executableBuilder;
};

