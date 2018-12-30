#pragma once

#include "Constraint.h"
#include "PhysicsEngine.h"

class SpringConstraint : public Constraint
{
public:
	SpringConstraint(PhysicsNode* a, PhysicsNode* b, const NCLVector3 globalOnA,
		const NCLVector3 globalOnB, float sprintConstant, float damping)
	{
		this->a = a;
		this->b = b;
		this->sprintConstant = sprintConstant;
		this->damping = damping;

		NCLVector3 dist = globalOnB - globalOnA;
		restDistance = dist.Length();
	}

	~SpringConstraint() {}

	virtual void ApplyImpulse() override
	{
		const NCLVector3 globalA = a->GetPosition();
		const NCLVector3 globalB = b->GetPosition();

		const NCLVector3 dist = globalB - globalA;
		NCLVector3 normalisedDist = dist;
		normalisedDist.Normalise();

		const float constraintMass = a->GetInverseMass() + b->GetInverseMass();

		if (constraintMass > 0.0f)
		{
			const NCLVector3 v0 = a->GetLinearVelocity();
			const NCLVector3 v1 = b->GetLinearVelocity();

			const float distanceOffset = dist.Length() - restDistance;
			const float baumgarteScalar = 0.3f;
			const float baumgarte = -(baumgarteScalar / (1.0f / 60.0f))
				* distanceOffset;

			float jn = (-(NCLVector3::Dot(v0 - v1, normalisedDist) + baumgarte)
				* sprintConstant) - (damping * (v0 - v1).Length());
			jn /= constraintMass;

			a->SetLinearVelocity(a->GetLinearVelocity() + normalisedDist
				* (jn * a->GetInverseMass()));
			b->SetLinearVelocity(b->GetLinearVelocity() - normalisedDist
				* (jn * b->GetInverseMass()));
		}

	}

	void SpringConstraint::DebugDraw() const override
	{
		//NCLDebug::DrawThickLineNDT(a->GetPosition(), b->GetPosition(), 0.02f, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
		//NCLDebug::DrawPointNDT(a->GetPosition(), 0.01f, Vector4(1.0f, 0.8f, 1.0f, 1.0f));
		//NCLDebug::DrawPointNDT(b->GetPosition(), 0.01f, Vector4(1.0f, 0.8f, 1.0f, 1.0f));
	}

private:
	PhysicsNode* a;
	PhysicsNode* b;

	float restDistance;
	float sprintConstant;
	float damping;
};

