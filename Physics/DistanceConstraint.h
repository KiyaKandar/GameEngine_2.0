#pragma once

#include "Constraint.h"
#include "PhysicsEngine.h"

class DistanceConstraint : public Constraint
{
public:
	DistanceConstraint(PhysicsNode* obj1, PhysicsNode* obj2,
		const NCLVector3& globalOnA, const NCLVector3& globalOnB)
	{
		pnodeA = obj1;
		pnodeB = obj2;

		NCLVector3 ab = globalOnB - globalOnA;
		targetLength = ab.length();

		NCLVector3 r1 = (globalOnA - pnodeA->getPosition());
		NCLVector3 r2 = (globalOnB - pnodeB->getPosition());
		relPosA = NCLMatrix3::transpose(pnodeA->getOrientation().toMatrix3()) * r1;
		relPosB = NCLMatrix3::transpose(pnodeB->getOrientation().toMatrix3()) * r2;
	}

	virtual void applyImpulse(float dt) override
	{
		NCLVector3 r1 = pnodeA->getOrientation().toMatrix3() * relPosA;
		NCLVector3 r2 = pnodeB->getOrientation().toMatrix3() * relPosB;

		NCLVector3 globalOnA = r1 + pnodeA->getPosition();
		NCLVector3 globalOnB = r2 + pnodeB->getPosition();

		NCLVector3 ab = globalOnB - globalOnA;
		NCLVector3 abn = ab;
		abn.normalise();

		NCLVector3 v0 = pnodeA->getLinearVelocity() + NCLVector3::cross(pnodeA->getAngularVelocity(), r1);
		NCLVector3 v1 = pnodeB->getLinearVelocity() + NCLVector3::cross(pnodeB->getAngularVelocity(), r2);

		float abnVel = NCLVector3::dot(v0 - v1, abn);

		float invConstraintMassLin = pnodeA->getInverseMass() + pnodeB->getInverseMass();

		float invConstraintMassRot = NCLVector3::dot(abn,
			NCLVector3::cross(pnodeA->getInverseInertia() * NCLVector3::cross(r1, abn), r1) +
			NCLVector3::cross(pnodeB->getInverseInertia() * NCLVector3::cross(r2, abn), r2));

		float constraintMass = invConstraintMassLin + invConstraintMassRot;

		if (constraintMass > 0.0f) {
			float b = 0.0f;

			float distance_offset = ab.length() - targetLength;
			float baumgarte_scalar = 0.1f;
			b = -(baumgarte_scalar / dt) * distance_offset;

			float jn = -(abnVel + b) / constraintMass;

			pnodeA->setLinearVelocity(pnodeA->getLinearVelocity() + abn*(pnodeA->getInverseMass() * jn));
			pnodeB->setLinearVelocity(pnodeB->getLinearVelocity() - abn*(pnodeB->getInverseMass() * jn));

			pnodeA->setAngularVelocity(pnodeA->getAngularVelocity() + pnodeA->getInverseInertia() * NCLVector3::cross(r1, abn * jn));
			pnodeB->setAngularVelocity(pnodeB->getAngularVelocity() - pnodeB->getInverseInertia() * NCLVector3::cross(r2, abn * jn));


		}
	}



protected:
	PhysicsNode *pnodeA, *pnodeB;

	float   targetLength;

	NCLVector3 relPosA;
	NCLVector3 relPosB;
};