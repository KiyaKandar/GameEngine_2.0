#include "ActionBuilder.h"

#include "ConditionalStatementBuilder.h"
#include "../Resource Management/XMLParser.h"
#include "../Communication/Message.h"
#include "../../Communication/MessagingService.h"
#include "../../Communication/DeliverySystem.h"
#include "../../Communication/Messages/TextMessage.h"
#include "../../Communication/Messages/RelativeTransformMessage.h"
#include "../../Communication/Messages/MoveCameraRelativeToGameObjectMessage.h"
#include "../../Communication/Messages/PreparePaintSurfaceMessage.h"
#include "../../Communication/Messages/PaintTrailForGameObjectMessage.h"
#include "../../Communication/Messages/AddScoreHolderMessage.h"
#include "../../Communication/SendMessageActionBuilder.h"
#include "../../Communication/Messages/PlaySoundMessage.h"

#include <iostream>
#include <string>

const std::string CONDITIONAL_STATEMENT = "Condition";
const std::string SEND_MESSAGE_STATEMENT = "SendMessage";

std::function<Executable(Node*)> ActionBuilder::executableBuilder
	= [](Node*) {return []() {}; };
std::vector<float> ActionBuilder::randomIntervals;

GameplayAction ActionBuilder::BuildAction(Node* node)
{
	std::vector<Condition> conditions;
	std::vector<Executable> executables;

	for (Node* section : node->children)
	{
		CompileActionSection(section, conditions, executables);
	}

	if (conditions.size() > 0)
	{
		return BuildFinalActionWithCondition(conditions, executables);
	}
	else
	{
		return BuildFinalAction(executables);
	}
}

TimedGameplayAction ActionBuilder::BuildTimedAction(Node* node)
{
	std::vector<Executable> executables;

	for (Node* section : node->children)
	{
		executables.push_back(CompileActionSectionWithoutCondition(section));
	}

	std::istringstream iss(node->name); 
	vector<string> tokens{ istream_iterator<string>{iss},
		istream_iterator<string>{} };

	if (tokens.size() > 1)
	{
		const float minInterval = stof(tokens[1]);
		const float maxInterval = stof(tokens[2]);
		int quantity = 1;

		if (tokens.size() == 4)
		{
			quantity = stoi(tokens[3]);
		}

		return buildTimedActionWithRandomisedInterval(quantity, minInterval, maxInterval, executables);
	}
	else
	{
		const float interval = std::stof(node->name);
		return buildTimedActionWithStaticInterval(interval, executables);
	}
}

GameplayAction ActionBuilder::BuildFinalActionWithCondition(std::vector<Condition>& conditions, std::vector<Executable>& executables)
{
	return [conditions, executables](Message message)
	{
		bool conditionsMet = true;

		for (Condition condition : conditions)
		{
			conditionsMet = conditionsMet && condition(message);
		}

		if (conditionsMet)
		{
			for (Executable executable : executables)
			{
				executable();
			}
		}
	};
}

GameplayAction ActionBuilder::BuildFinalAction(std::vector<Executable>& executables)
{
	return [executables](Message message)
	{
		for (Executable executable : executables)
		{
			executable();
		}
	};
}

void ActionBuilder::CompileActionSection(Node* section, std::vector<Condition>& conditions, std::vector<Executable>& executables)
{
	if (section->nodeType == CONDITIONAL_STATEMENT)
	{
		conditions.push_back(BuildIfStatement(section));
	}
	else
	{
		executables.push_back(CompileActionSectionWithoutCondition(section));
	}
}

Executable ActionBuilder::CompileActionSectionWithoutCondition(Node* section)
{
	return executableBuilder(section);
}

void ActionBuilder::SetExecutableBuilder(std::function<Executable(Node*)> executableBuilder)
{
	ActionBuilder::executableBuilder = executableBuilder;
}

Condition ActionBuilder::BuildIfStatement(Node* node)
{
	if (node->name == "OR")
	{
		return ConditionalStatementBuilder::BuildOrCondition(node);
	}
	else if (node->name == "AND")
	{
		return ConditionalStatementBuilder::BuildAndCondition(node);
	}
	else
	{
		return ConditionalStatementBuilder::BuildSingleIfCondition(node);
	}
}

float ActionBuilder::generateRandomInterval(const float minInterval, const float maxInterval)
{
	const float random = ((float)rand()) / (float)RAND_MAX;
	const float range = maxInterval - minInterval;
	return (random * range) + minInterval;
}

TimedGameplayAction ActionBuilder::buildTimedActionWithStaticInterval(const float interval,
	const std::vector<Executable>& executables)
{
	return [interval, executables](float& timer)
	{
		if (timer >= interval)
		{
			timer = 0.0f;

			for (Executable executable : executables)
			{
				executable();
			}
		}
	};
}

TimedGameplayAction ActionBuilder::buildTimedActionWithRandomisedInterval(const int quantity, const float minInterval,
	const float maxInterval, const std::vector<Executable>& executables)
{
	randomIntervals.push_back(ActionBuilder::generateRandomInterval(minInterval, maxInterval));

	return[quantity, minInterval, maxInterval, executables, storedInterval = &randomIntervals.back()](float& timer)
	{
		for (int i = 0; i < quantity; ++i)
		{
			if (timer >= *storedInterval)
			{
				timer = 0.0f;
				*storedInterval = ActionBuilder::generateRandomInterval(minInterval, maxInterval);

				for (Executable executable : executables)
				{
					executable();
				}
			}
		}
	};
}
