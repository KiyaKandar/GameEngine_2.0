#include "ConditionalStatementBuilder.h"

#include "../Communication/Message.h"
#include "../Resource Management/XMLParser.h"

Condition ConditionalStatementBuilder::BuildOrCondition(Node* node)
{
	std::vector<Node> children;

	for (Node* child : node->children)
	{
		children.push_back(*child);
	}

	return [children](Message message)
	{
		bool condition = false;

		for (Node childCondition : children)
		{
			condition = condition || message.GetDataField(childCondition.nodeType) == childCondition.value;
		}

		return condition;
	};
}

Condition ConditionalStatementBuilder::BuildAndCondition(Node* node)
{
	std::vector<Node> children;

	for (Node* child : node->children)
	{
		children.push_back(*child);
	}

	return [children](Message message)
	{
		bool condition = true;

		for (Node childCondition : children)
		{
			condition = condition && message.GetDataField(childCondition.nodeType) == childCondition.value;
		}

		return condition;
	};
}

Condition ConditionalStatementBuilder::BuildSingleIfCondition(Node* node)
{
	Node* conditionNode = node->children[0];

	return [conditionNode = *conditionNode](Message message)
	{
		return message.GetDataField(conditionNode.nodeType) == conditionNode.value;
	};
}
