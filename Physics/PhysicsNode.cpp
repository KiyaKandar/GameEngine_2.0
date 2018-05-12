#include "PhysicsNode.h"
#include "../Communication/Messages/MoveCameraRelativeToGameObjectMessage.h"
#include "../Communication/DeliverySystem.h"

static const NCLVector3 gravity = NCLVector3(0.0f, -9.8f, 0.0f);
static const float MAX_SPEED = 50.0f;

NCLVector3 interpolate(NCLVector3 a, NCLVector3 b, float factor)
{
	return a + ((b - a) * factor);
}

void PhysicsNode::integrateForVelocity(float dt)
{
	if (invMass > 0.f) 
	{
		linVelocity += gravity * dt;
	}

	if(!constantAcceleration)
	{
		force = appliedForce;
		acceleration = force * invMass;
	}

	linVelocity += acceleration * dt;

	linVelocity = linVelocity * damping;

	if(linVelocity.length() > MAX_SPEED)
	{
		linVelocity = linVelocity.normalise() * MAX_SPEED;
	}

	angVelocity += invInertia * torque * dt;

	angVelocity = angVelocity * damping;

	appliedForce.toZero();
}

void PhysicsNode::integrateForPosition(float dt)
{
	position += linVelocity * dt;

	//if (constantForce)
	//{
	//	msCounter += dt;
	//	float factor = (msCounter - deadReckoningState.timeStamp) / 15.0f;

	//	if (factor <= 1.0f && factor >= 0.0f)
	//	{
	//		position = interpolate(position, deadReckoningState.position, factor);

	//		linVelocity = interpolate(linVelocity, deadReckoningState.linearVelocity, factor);

	//		acceleration = interpolate(acceleration, deadReckoningState.linearAcceleration, factor);
	//	}
	//}

	orientation = orientation + Quaternion(angVelocity * dt * .5f, 0.f) * orientation;

	orientation.normalise();

	worldTransform = orientation.toMatrix();

	worldTransform.setPositionVector(position);
	fireOnUpdateCallback();

	//if (parent->getName() == MoveCameraRelativeToGameObjectMessage::resourceName)
	//{
	//	DeliverySystem::getPostman()->insertMessage(UpdatePositionMessage("NetworkClient", parent->getName(), position));
	//}
}

