#pragma once

#include "ActionBuilder.h"

class ConditionalStatementBuilder
{
public:
	static Condition BuildOrCondition(Node* node);
	static Condition BuildAndCondition(Node* node);
	static Condition BuildSingleIfCondition(Node* node);
};

