#include "MinionDeadReckoning.h"
#include "../Physics/PhysicsNode.h"

const float MAX_INTERPOLATION_DISTANCE = 50.0f;

MinionDeadReckoning::MinionDeadReckoning(MinionKinematicState prediction)
{
	this->prediction = prediction;
}

void MinionDeadReckoning::blendStates(PhysicsNode* node)
{
	const float interpolationFactor = calculateInterpolationFactor(node->GetPosition());
	node->SetPosition(NCLVector3::interpolate(node->GetPosition(), prediction.position, interpolationFactor));
}

void MinionDeadReckoning::predictPosition(float deltaTime)
{
	prediction.linearVelocity += prediction.linearAcceleration * deltaTime;
	prediction.position += prediction.linearVelocity * deltaTime;
}

float MinionDeadReckoning::calculateInterpolationFactor(const NCLVector3& originalPosition)
{
	float factor = (prediction.position - originalPosition).length();
	factor /= MAX_INTERPOLATION_DISTANCE;

	if (factor > 1.0f)
	{
		factor = 1.0f;
	}

	return factor;
}
