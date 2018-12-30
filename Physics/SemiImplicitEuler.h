#pragma once

#include "PhysicsNode.h"
#include "PhysicsEngine.h"

class SemiImplicitEuler
{
public:
	static NCLVector3 UpdateLinearVelocity(PhysicsNode* node, const float& dt)
	{
		NCLVector3 velocity = node->GetLinearVelocity();

		//Apply Gravity
		if (node->GetInverseMass() > 0.0f)
		{
			velocity += NCLVector3(0.0f, -9.81f, 0.0f) * dt;
		}

		if (!node->constantAcceleration)
		{
			node->SetForce(node->GetAppliedForce());
			node->SetAcceleration(node->GetForce() * node->GetInverseMass());
			node->SetAppliedForce(NCLVector3());
		}

		//Semi-Implicit Euler Integration
		velocity += node->GetAcceleration() * dt;

		//Damping
		velocity = velocity * node->GetDamping();

		return velocity;
	}

	static NCLVector3 UpdateAngularVelocity(const PhysicsNode* node, const float& dt)
	{
		NCLVector3 angularVelocity = node->GetAngularVelocity();

		//Angular Rotation 
		/*
		mass     -> torque
		velocity -> rotational velocity
		position -> orientation
		*/
		angularVelocity += node->GetInverseInertia() * node->GetTorque() * dt;

		//Damping
		angularVelocity = angularVelocity * node->GetDamping();

		return angularVelocity;
	}

	static NCLVector3 UpdateDisplacement(const PhysicsNode* node, const float& dt)
	{
		return node->GetPosition() + node->GetLinearVelocity() * dt;
	}

	static Quaternion UpdateOrentation(const PhysicsNode* node, const float& dt)
	{
		return node->GetOrientation() + Quaternion(node->GetAngularVelocity() * dt * 0.5f, 0.0f)
			* node->GetOrientation();
	}
};

