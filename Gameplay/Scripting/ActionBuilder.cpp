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

const std::string CONDITIONAL_STATEMENT = "Condition";
const std::string SEND_MESSAGE_STATEMENT = "SendMessage";

std::function<Executable(Node*)> ActionBuilder::executableBuilder
	= [](Node*) {return []() {}; };

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

	float interval = std::stof(node->name);

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