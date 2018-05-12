#pragma once

#include "PhysicsNode.h"

#include "../Utilities/Maths/Vector3.h"

/* A contact constraint is actually the summation of a distance constraint to handle the main collision (normal)
along with two friction constraints going along the axes perpendicular to the collision
normal.
*/
struct ContactPoint
{
	//Amount of additional energy to add to the system
	// -- Summation of Baumgarte error correction and elasticity
	float	bTerm;

	//Collision Normal and Penetration depth
	NCLVector3 colNormal;
	float	colPenetration;

	NCLVector3 relPosA;			//Position relative to objectA
	NCLVector3 relPosB;			//Position relative to objectB

								//Solver - Total force added this frame
								// - Used to clamp contact constraint over the course of the entire solver
								//   to expected bounds.
	float   sumImpulseContact;
	NCLVector3 sumImpulseFriction;
};

class Manifold
{
public:
	Manifold();
	~Manifold();

	//Initiate for collision pair
	void initiate(PhysicsNode* nodeA, PhysicsNode* nodeB);

	//Called whenever a new collision contact between A & B are found
	void addContact(const NCLVector3& globalOnA, const NCLVector3& globalOnB, const NCLVector3& _normal, const float& _penetration);

	//Sequentially solves each contact constraint
	void applyImpulse();
	void preSolverStep(float dt);


	//Get the physics objects
	PhysicsNode* nodeA() 
	{ 
		return pnodeA; 
	}
	PhysicsNode* nodeB() 
	{ 
		return pnodeB; 
	}

protected:
	void solveContactPoint(ContactPoint& c);
	void updateConstraint(ContactPoint& c, float deltaTime);

public:
	PhysicsNode*				pnodeA;
	PhysicsNode*				pnodeB;
	std::vector<ContactPoint>	contactPoints;
};

