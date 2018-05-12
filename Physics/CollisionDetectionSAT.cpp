#include "CollisionDetectionSAT.h"
#include "GeometryUtils.h"
#include "PhysicsNode.h"
#include "Manifold.h"

using namespace GeometryUtils;


CollisionDetectionSAT::CollisionDetectionSAT()
{
}

void CollisionDetectionSAT::beginNewPair(PhysicsNode * objA, PhysicsNode * objB, CollisionShape * shapeA, CollisionShape * shapeB)
{
	possibleColAxes.clear();

	pnodeA = objA;
	pnodeB = objB;
	cshapeA = objA->getCollisionShape();
	cshapeB = objB->getCollisionShape();

	colliding = false;
}



bool CollisionDetectionSAT::areColliding(CollisionData* out_coldata)
{
	if (!cshapeA || !cshapeB)
		return false;

	colliding = false;
	possibleColAxes.clear();

	std::vector<NCLVector3> axes1, axes2;

	cshapeA->getCollisionAxes(pnodeB, axes1);
	for (const NCLVector3& axis : axes1) {
		addPossibleCollisionAxis(axis);
	}

	cshapeB->getCollisionAxes(pnodeA, axes2);
	for (const NCLVector3& axis : axes2) {
		addPossibleCollisionAxis(axis);
	}

	//change when using more complicated geometry
	//just cuboids for now
	for (const NCLVector3& norm1 : axes1) {
		for (const NCLVector3& norm2 : axes2) {
			addPossibleCollisionAxis(NCLVector3::cross(norm1, norm2).normalise());
		}
	}

	CollisionData cur_colData;

	bestColData.penetration = -FLT_MAX;
	for (const NCLVector3& axis : possibleColAxes) {
		if (!checkCollisionAxis(axis, cur_colData))
			return false;

		if (cur_colData.penetration >= bestColData.penetration) {
			bestColData = cur_colData;
		}
	}

	if (out_coldata)
		*out_coldata = bestColData;

	colliding = true;
	return true;
}



void CollisionDetectionSAT::genContactPoints(Manifold * out_manifold)
{
	if (!out_manifold || !colliding)
		return;

	if (bestColData.penetration >= 0.f) {
		return;
	}

	std::list<NCLVector3> polygon1, polygon2;
	NCLVector3 normal1, normal2;
	std::vector<Plane> adjPlanes1, adjPlanes2;

	cshapeA->getIncidentReferencePolygon(bestColData.normal, polygon1, normal1, adjPlanes1);
	cshapeB->getIncidentReferencePolygon(-bestColData.normal, polygon2, normal2, adjPlanes2);

	if (polygon1.size() == 0 || polygon2.size() == 0) {
		return;
	}
	else if (polygon1.size() == 1) {
		out_manifold->addContact(polygon1.front(), polygon1.front() + bestColData.normal * bestColData.penetration, bestColData.normal, bestColData.penetration);
	}
	else if (polygon2.size() == 1) {
		out_manifold->addContact(polygon2.front() - bestColData.normal * bestColData.penetration, polygon2.front(), bestColData.normal, bestColData.penetration);
	}
	else {
		bool flipped = fabs(NCLVector3::dot(bestColData.normal, normal1)) < fabs(NCLVector3::dot(bestColData.normal, normal2));

		if (flipped) {
			std::swap(polygon1, polygon2);
			std::swap(normal1, normal2);
			std::swap(adjPlanes1, adjPlanes2);
		}

		if (adjPlanes1.size() > 0) {
			sutherlandHodgmanClipping(polygon2, adjPlanes1.size(), &adjPlanes1[0], &polygon2, false);
		}

		Plane refPlane = Plane(-normal1, -NCLVector3::dot(-normal1, polygon1.front()));

		sutherlandHodgmanClipping(polygon2, 1, &refPlane, &polygon2, true);

		for (const NCLVector3& point : polygon2) {
			NCLVector3 pointDiff = point - getClosestPointPolygon(point, polygon1);
			float contact_penetration = NCLVector3::dot(pointDiff, bestColData.normal);

			NCLVector3 globalOnA = point;
			NCLVector3 globalOnB = point - bestColData.normal * contact_penetration;

			if (flipped) {
				contact_penetration = -contact_penetration;
				globalOnA = point + bestColData.normal * contact_penetration;
				globalOnB = point;
			}

			if (contact_penetration < 0.0f) {
				out_manifold->addContact(globalOnA, globalOnB, bestColData.normal, contact_penetration);
			}
		}
	}
}

bool CollisionDetectionSAT::addPossibleCollisionAxis(NCLVector3 axis)
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

bool CollisionDetectionSAT::checkCollisionAxis(const NCLVector3 & axis, CollisionData & out_coldata)
{
	NCLVector3 min1, min2, max1, max2;

	cshapeA->getMinMaxVertexOnAxis(axis, min1, max1);
	cshapeB->getMinMaxVertexOnAxis(axis, min2, max2);

	float A = NCLVector3::dot(axis, min1);
	float B = NCLVector3::dot(axis, max1);
	float C = NCLVector3::dot(axis, min2);
	float D = NCLVector3::dot(axis, max2);

	if (A <= C && B >= C) {
		out_coldata.normal = axis;
		out_coldata.penetration = C - B;
		out_coldata.pointOnPlane = max1 + out_coldata.normal * out_coldata.penetration;
		return true;
	}

	if (C <= A && D >= A) {
		out_coldata.normal = -axis;
		out_coldata.penetration = A - D;
		out_coldata.pointOnPlane = min1 + out_coldata.normal * out_coldata.penetration;
		return true;
	}

	return false;
}
