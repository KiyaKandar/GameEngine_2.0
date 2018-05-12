#include "SphereCollisionShape.h"
#include "PhysicsNode.h"



SphereCollisionShape::SphereCollisionShape()
{
	radius = 1.f;
}

SphereCollisionShape::SphereCollisionShape(float newRadius)
{
	radius = newRadius;
}


SphereCollisionShape::~SphereCollisionShape()
{
}

void SphereCollisionShape::debugDraw()
{
	DeliverySystem::getPostman()->insertMessage(DebugSphereMessage("RenderingSystem", parent()->getPosition(), radius, NCLVector3(1, 0, 0)));
}

NCLMatrix3 SphereCollisionShape::buildInverseInertia(float invMass) const
{
	float i = 2.5f * invMass / (radius * radius);

	NCLMatrix3 inertia;
	inertia._11 = i;
	inertia._22 = i;
	inertia._33 = i;

	return inertia;
}

void SphereCollisionShape::getCollisionAxes(const PhysicsNode * otherObject, std::vector<NCLVector3>& out_axes) const
{
	NCLVector3 dir = (otherObject->getPosition() - parent()->getPosition()).normalise();

	NCLVector3 p1 = parent()->getPosition();
	NCLVector3 p2 = otherObject->getCollisionShape()->getClosestPoint(p1);

	out_axes.push_back((p1 - p2).normalise());
}

NCLVector3 SphereCollisionShape::getClosestPoint(const NCLVector3 & point) const
{
	NCLVector3 diff = (point - parent()->getPosition()).normalise();
	return parent()->getPosition() + diff * radius;
}

void SphereCollisionShape::getMinMaxVertexOnAxis(const NCLVector3 & axis, NCLVector3 & out_min, NCLVector3 & out_max) const
{
	out_min = parent()->getPosition() - axis * radius;
	out_max = parent()->getPosition() + axis * radius;
}

void SphereCollisionShape::getIncidentReferencePolygon(const NCLVector3 & axis, std::list<NCLVector3>& out_face, NCLVector3 & out_normal, std::vector<Plane>& out_adjacent_planes) const
{
	out_face.push_back(parent()->getPosition() + axis * radius);
	out_normal = axis;
}
