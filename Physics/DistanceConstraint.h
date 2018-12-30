/*
	Manages a distance constraint between two objects, ensuring the two objects never
	seperate. It works on the velocity level, enforcing the constraint:
		dot([(velocity of B) - (velocity of A)], normal) = zero
	
	Which is the same as saying, if the velocity of the two objects in the direction of the constraint is zero
	then we can assert that the two objects will not move further or closer together and thus satisfy the constraint.

*/

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

		//Set the preferred distance of the constraint to enforce 
		// (ONLY USED FOR BAUMGARTE)
		// - Ideally we only ever work at the velocity level, so satifying (velA-VelB = 0)
		//   is enough to ensure the distance never changes.
		NCLVector3 ab = globalOnB - globalOnA;
		targetLength = ab.Length();

		//Get the local points (un-rotated) on the two objects where the constraint should
		// be attached relative to the objects center. So when we rotate the objects
		// the constraint attachment point will rotate with it.
		NCLVector3 r1 = (globalOnA - pnodeA->GetPosition());
		NCLVector3 r2 = (globalOnB - pnodeB->GetPosition());
		relPosA = NCLMatrix3::Transpose(pnodeA->GetOrientation().ToMatrix3()) * r1;
		relPosB = NCLMatrix3::Transpose(pnodeB->GetOrientation().ToMatrix3()) * r2;
	}

	//Solves the constraint and applies a velocity impulse to the two
	// objects in order to satisfy the constraint.
	virtual void ApplyImpulse() override
	{
		//Compute current constraint vars based on object A/B’s position / rotation
		const NCLVector3 r1 = pnodeA->GetOrientation().ToMatrix3() * relPosA;
		const NCLVector3 r2 = pnodeB->GetOrientation().ToMatrix3() * relPosB;

		//Get the global contact points in world space
		const NCLVector3 globalOnA = r1 + pnodeA->GetPosition();
		const NCLVector3 globalOnB = r2 + pnodeB->GetPosition();

		//Get the vector between the two contact points
		const NCLVector3 ab = globalOnB - globalOnA;
		NCLVector3 abn = ab;
		abn.Normalise(); 
		
		//Compute the velocity of objects A and B at the point of contact
		const NCLVector3 v0 = pnodeA->GetLinearVelocity()
			+ NCLVector3::Cross(pnodeA->GetAngularVelocity(), r1);
		const NCLVector3 v1 = pnodeB->GetLinearVelocity()
			+ NCLVector3::Cross(pnodeB->GetAngularVelocity(), r2);

		//Relative velocity in constraint direction
		const float abnVel = NCLVector3::dot(v0 - v1, abn);

		//Compute the ’mass ’ of the constraint e.g. How difficult it 
		//is to move the two objects in the direction of the constraint
		const float invConstraintMassLin = pnodeA->GetInverseMass()
			+ pnodeB->GetInverseMass();

		const float invConstraintMassRot = NCLVector3::dot(abn,
			NCLVector3::Cross(pnodeA->GetInverseInertia()
			* NCLVector3::Cross(r1, abn), r1)
			+ NCLVector3::Cross(pnodeB->GetInverseInertia()
			* NCLVector3::Cross(r2, abn), r2));

		const float constraintMass = invConstraintMassLin + invConstraintMassRot;

		if (constraintMass > 0.0f)
		{
			//Baumgarte Offset ( Adds energy to the system to counter
			//slight solving errors that accumulate over time - known
			//as ’constraint drift ’)
			//The key is to find a nice value that is small enough
			//not to cause objects to explode but also enough to make
			//sure all constraints / will / be satisfied . This value
			//(0.1) will change based on your physics objects ,
			//timestep etc . , and also how many constraints you are
			//chaining together .

			// -Optional -
			float b = 0.0f;
			const float distance_offset = ab.Length() - targetLength; 
			const float baumgarte_scalar = 0.3f;
			b = -(baumgarte_scalar / (1.0f / 60.0f)) * distance_offset;
			// -Eof Optional -

			//Compute velocity impulse (jn)
			//In order to satisfy the distance constraint we need
			//to apply forces to ensure the relative velocity
			//( abnVel ) in the direction of the constraint is zero .
			//So we take inverse of the current rel velocity and
			//multiply it by how hard it will be to move the objects .
			//Note : We also add in any extra energy to the system
			//here , e.g. baumgarte (and later elasticity )

			const float jn = -(abnVel + b) / constraintMass;

			//Apply linear velocity impulse 
			pnodeA->SetLinearVelocity(pnodeA->GetLinearVelocity()
				+ abn * (pnodeA->GetInverseMass() * jn));
			pnodeB->SetLinearVelocity(pnodeB->GetLinearVelocity()
				- abn * (pnodeB->GetInverseMass() * jn));

			//Apply rotational velocity impulse 
			pnodeA->SetAngularVelocity(pnodeA->GetAngularVelocity()
				+ pnodeA->GetInverseInertia()
				* NCLVector3::Cross(r1, abn * jn));
			pnodeB->SetAngularVelocity(pnodeB->GetAngularVelocity()
				- pnodeB->GetInverseInertia()
				* NCLVector3::Cross(r2, abn * jn));
		}
	}

	//Draw the constraint visually to the screen for debugging
	virtual void DebugDraw() const
	{
		NCLVector3 globalOnA = pnodeA->GetOrientation().ToMatrix3() * relPosA + pnodeA->GetPosition();
		NCLVector3 globalOnB = pnodeB->GetOrientation().ToMatrix3() * relPosB + pnodeB->GetPosition();

		//NCLDebug::DrawThickLine(globalOnA, globalOnB, 0.02f, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		//NCLDebug::DrawPointNDT(globalOnA, 0.05f, Vector4(1.0f, 0.8f, 1.0f, 1.0f));
		//NCLDebug::DrawPointNDT(globalOnB, 0.05f, Vector4(1.0f, 0.8f, 1.0f, 1.0f));
	}

protected:
	PhysicsNode *pnodeA, *pnodeB;

	float   targetLength;

	NCLVector3 relPosA;
	NCLVector3 relPosB;
};