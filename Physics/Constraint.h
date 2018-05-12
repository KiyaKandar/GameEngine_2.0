#pragma once

#include "PhysicsNode.h"

class Constraint
{
public:
	Constraint() {};
	~Constraint() {};

	virtual void applyImpulse(float dt) = 0;

	virtual void preSolverStep(float dt) {}
};

