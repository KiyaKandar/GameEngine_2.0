#include "CollisionDetectionSAT.h"

#include "GeometryUtils.h"
#include "PhysicsNode.h"
#include "Manifold.h"

using namespace GeometryUtils;

CollisionDetectionSAT::CollisionDetectionSAT()
{
}

void CollisionDetectionSAT::BeginNewPair(
	PhysicsNode* obj1,
	PhysicsNode* obj2,
	CollisionShape* shape1,
	CollisionShape* shape2)
{
	possibleColAxes.clear();

	pnodeA = obj1;
	pnodeB = obj2;
	cshapeA = shape1;
	cshapeB = shape2;

	areColliding = false;
}



bool CollisionDetectionSAT::AreColliding(CollisionData* out_coldata)
{
	if (!cshapeA || !cshapeB)
		return false;

	areColliding = false;
	possibleColAxes.clear();

	std::vector<NCLVector3> axes1, axes2;

	cshapeA->GetCollisionAxes(pnodeB, axes1);
	for (const NCLVector3& axis : axes1)
		AddPossibleCollisionAxis(axis);

	cshapeB->GetCollisionAxes(pnodeA, axes2);
	for (const NCLVector3& axis : axes2)
		AddPossibleCollisionAxis(axis);

	for (const NCLVector3& norm1 : axes1)
	{
		for (const NCLVector3& norm2 : axes2)
		{
			AddPossibleCollisionAxis(
				NCLVector3::cross(norm1, norm2).normalise());
		}
	}

	CollisionData cur_colData;

	bestColData._penetration = -FLT_MAX;
	for (const NCLVector3& axis : possibleColAxes)
	{
		if (!CheckCollisionAxis(axis, cur_colData))
			return false;

		if (cur_colData._penetration >= bestColData._penetration)
		{
			bestColData = cur_colData;
		}
	}

	if (out_coldata) *out_coldata = bestColData;

	areColliding = true;
	return true;
}


bool CollisionDetectionSAT::CheckCollisionAxis(const NCLVector3& axis, CollisionData& out_coldata)
{
	//Overlap Test
	// Points go: 
	//          +-------------+
	//  +-------|-----+   2   |
	//  |   1   |     |       |
	//  |       +-----|-------+
	//  +-------------+
	//  A ------C --- B ----- D 
	//
	//	IF	 A < C AND B > C (Overlap in order object 1 -> object 2)
	//	IF	 C < A AND D > A (Overlap in order object 2 -> object 1)

	NCLVector3 min1, min2, max1, max2;

	cshapeA->GetMinMaxVertexOnAxis(axis, min1, max1);
	cshapeB->GetMinMaxVertexOnAxis(axis, min2, max2);

	const float A = NCLVector3::dot(axis, min1);
	const float B = NCLVector3::dot(axis, max1);
	const float C = NCLVector3::dot(axis, min2);
	const float D = NCLVector3::dot(axis, max2);

	if (A <= C && B >= C)
	{
		out_coldata._normal = axis;
		out_coldata._penetration = C - B;
		out_coldata._pointOnPlane = max1 + out_coldata._normal * out_coldata._penetration;
		return true;
	}

	if (C <= A && D >= A)
	{
		out_coldata._normal = -axis;
		out_coldata._penetration = A - D;
		out_coldata._pointOnPlane = min1 + out_coldata._normal * out_coldata._penetration;
		return true;
	}

	return false;
}






void CollisionDetectionSAT::GenContactPoints(Manifold* out_manifold)
{
	if (!out_manifold || !areColliding)
		return;

	if (bestColData._penetration >= 0.0f)
		return;

	//Get required face info for two shapes around collision normal
	std::list<NCLVector3> polygon1, polygon2;
	NCLVector3 normal1, normal2;
	std::vector<Plane> adjPlanes1, adjPlanes2;

	cshapeA->GetIncidentReferencePolygon(
		bestColData._normal, polygon1, normal1, adjPlanes1);
	cshapeB->GetIncidentReferencePolygon(
		-bestColData._normal, polygon2, normal2, adjPlanes2);

	//if either shape1 or shape2 returned a single point, then it
	//must be on a curbe and thus the only contact point to generate 
	//is already available
	if (polygon1.size() == 0 || polygon2.size() == 0)
	{
		return; //no points therefore no contact points
	}
	else if (polygon1.size() == 1)
	{
		out_manifold->AddContact(
			polygon1.front(),
			polygon1.front() + bestColData._normal
		* bestColData._penetration, bestColData._normal,
			bestColData._penetration);
	}
	else if (polygon2.size() == 1)
	{
		out_manifold->AddContact(
			polygon2.front() - bestColData._normal
			* bestColData._penetration,
			polygon2.front(),
			bestColData._normal,
			bestColData._penetration);
	}
	else
	{
		//Use clipping to cut down incident face to fit inside reference
		//planes using surrounding face planes

		//First we need to know if have to flip incident and reference
		//faces around for clipping

		bool flipped = fabs(NCLVector3::dot(bestColData._normal, normal1))
			< fabs(NCLVector3::dot(bestColData._normal, normal2));

		if (flipped)
		{
			std::swap(polygon1, polygon2);
			std::swap(normal1, normal2);
			std::swap(adjPlanes1, adjPlanes2);
		}

		//clip the incident face to the adjacent edges of reference face

		if (adjPlanes1.size() > 0)
			SutherlandHodgmanClipping(polygon2, adjPlanes1.size(),
				&adjPlanes1[0], &polygon2, false);

		//Finally clip and remove any contact points above the reference plane
		Plane refPlane =
			Plane(-normal1, -NCLVector3::dot(-normal1, polygon1.front()));
		SutherlandHodgmanClipping(polygon2, 1, &refPlane, &polygon2, true);

		//Now left with selection of valid contact points for manifold

		for (const NCLVector3& point : polygon2)
		{
			//Compute distance to reference plane

			NCLVector3 pointDiff =
				point - GetClosestPointPolygon(point, polygon1);
			float contact_peneration =
				NCLVector3::dot(pointDiff, bestColData._normal);

			//Set contact data

			NCLVector3 globalOnA = point;
			NCLVector3 globalOnB =
				point - bestColData._normal * contact_peneration;

			//If we flipped incident and reference planes,
			//we will need to flip it back for manifold.
			
			if (flipped)
			{
				contact_peneration = -contact_peneration;
				globalOnA =
					point + bestColData._normal * contact_peneration;

				globalOnB = point;
			}

			//Final sanity check that contact point is actual point of contact
			//and not a clipping bug

			if (contact_peneration < 0.0f)
			{
				out_manifold->AddContact(
					globalOnA, globalOnB,
					bestColData._normal,
					contact_peneration);
			}
		}
	}
}

bool CollisionDetectionSAT::AddPossibleCollisionAxis(NCLVector3 axis)
{
	const float epsilon = 1e-6f;

	//is axis 0,0,0??
	if (NCLVector3::dot(axis, axis) < epsilon)
		return false;

	axis.normalise();

	for (const NCLVector3& p_axis : possibleColAxes)
	{
		//Is axis very close to the same as a previous axis already in the list of axes??
		if (NCLVector3::dot(axis, p_axis) >= 1.0f - epsilon)
			return false;
	}

	possibleColAxes.push_back(axis);
	return true;
}


