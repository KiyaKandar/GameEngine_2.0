#pragma once

#include "../Utilities/Maths/Vector3.h"
#include "../Utilities/Maths/Quaternion.h"

class PhysicsNode;

struct MinionKinematicState
{
	int minionIndex;
	int padding;

	NCLVector3 position;
	NCLVector3 linearVelocity;
	NCLVector3 linearAcceleration;
};

class MinionDeadReckoning
{
public:
	explicit MinionDeadReckoning(MinionKinematicState prediction);

	MinionDeadReckoning()
	{
	}

	~MinionDeadReckoning()
	{
	}

	void BlendStates(PhysicsNode* node) const;
	void PredictPosition(float deltaTime);

private:
	MinionKinematicState prediction;
	float CalculateInterpolationFactor(const NCLVector3& originalPosition) const;
};
