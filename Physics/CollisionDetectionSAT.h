#pragma once
#include "CollisionShape.h"

struct CollisionData
{
	NCLVector3		normal;
	float		penetration;
	NCLVector3		pointOnPlane;
};

class PhysicsNode;
class Manifold;

class CollisionDetectionSAT
{
public:
	CollisionDetectionSAT();

	void beginNewPair(
		PhysicsNode* objA,
		PhysicsNode* objB,
		CollisionShape* shapeA,
		CollisionShape* shapeB);

	bool areColliding(CollisionData* out_coldata = NULL);

	void genContactPoints(Manifold* out_manifold);

protected:
	bool addPossibleCollisionAxis(NCLVector3 axis);

	bool checkCollisionAxis(const NCLVector3& axis, CollisionData& coldata);

private:
	
	const PhysicsNode*		pnodeA;
	const PhysicsNode*		pnodeB;

	
	const CollisionShape*	cshapeA;
	const CollisionShape*	cshapeB;

	std::vector<NCLVector3>	possibleColAxes;

	bool					colliding;
	CollisionData			bestColData;
};