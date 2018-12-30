#include "DeadReckoning.h"
#include "../Physics/PhysicsNode.h"

const float MAX_INTERPOLATION_DISTANCE = 50.0f;

DeadReckoning::DeadReckoning(KinematicState prediction)
{
	this->prediction = prediction;
}

void DeadReckoning::BlendStates(PhysicsNode* node) const
{
	const float interpolationFactor = CalculateInterpolationFactor(node->GetPosition());
	node->SetPosition(NCLVector3::interpolate(node->GetPosition(), prediction.position, interpolationFactor));
}

void DeadReckoning::PredictPosition(float deltaTime)
{
	prediction.linearVelocity += prediction.linearAcceleration * deltaTime;
	prediction.position += prediction.linearVelocity * deltaTime;
}

float DeadReckoning::CalculateInterpolationFactor(const NCLVector3& originalPosition) const
{
	float factor = (prediction.position - originalPosition).length();
	factor /= MAX_INTERPOLATION_DISTANCE;

	if (factor > 1.0f)
	{
		factor = 1.0f;
	}

	return factor;
}
