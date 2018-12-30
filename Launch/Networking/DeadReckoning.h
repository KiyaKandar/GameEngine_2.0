#pragma once
#include "../Utilities/Maths/Vector3.h"
#include "../Utilities/Maths/Quaternion.h"

class PhysicsNode;

struct KinematicState
{
	int clientID;

	NCLVector3 position;
	NCLVector3 linearVelocity;
	NCLVector3 linearAcceleration;
};

class DeadReckoning
{
public:
	explicit DeadReckoning(KinematicState prediction);

	DeadReckoning()
	{
	}

	~DeadReckoning()
	{
	}

	void BlendStates(PhysicsNode* node) const;
	void PredictPosition(float deltaTime);

private:
	KinematicState prediction;
	float CalculateInterpolationFactor(const NCLVector3& originalPosition) const;
};
