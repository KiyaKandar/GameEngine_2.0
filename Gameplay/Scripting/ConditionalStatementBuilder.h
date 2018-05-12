#pragma once

#include "ActionBuilder.h"

class ConditionalStatementBuilder
{
public:
	static Condition buildOrCondition(Node* node);
	static Condition buildAndCondition(Node* node);
	static Condition buildSingleIfCondition(Node* node);
};

