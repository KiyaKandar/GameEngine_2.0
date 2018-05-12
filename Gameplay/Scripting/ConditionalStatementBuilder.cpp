#include "ConditionalStatementBuilder.h"

#include "../Communication/Message.h"
#include "../Resource Management/XMLParser.h"

Condition ConditionalStatementBuilder::buildOrCondition(Node* node)
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
			condition = condition || message.getDataField(childCondition.nodeType) == childCondition.value;
		}

		return condition;
	};
}

Condition ConditionalStatementBuilder::buildAndCondition(Node* node)
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
			condition = condition && message.getDataField(childCondition.nodeType) == childCondition.value;
		}

		return condition;
	};
}

Condition ConditionalStatementBuilder::buildSingleIfCondition(Node* node)
{
	Node* conditionNode = node->children[0];

	return [conditionNode = *conditionNode](Message message)
	{
		return message.getDataField(conditionNode.nodeType) == conditionNode.value;
	};
}
