#pragma once
#include "CollisionShape.h"

class DebugLineMessage;
class DebugSphereMessage;

class SphereCollisionShape : public CollisionShape
{
public:
	SphereCollisionShape();
	SphereCollisionShape(float newRadius);
	~SphereCollisionShape();

	void setScale(NCLVector3 scale, float invMass) override
	{
		radius = scale.x;
		buildInverseInertia(invMass);
	}
	float getRadius()
	{
		return radius;
	}

	void debugDraw(std::vector<DebugLineMessage>& lineMessages, std::vector<DebugSphereMessage>& sphereMessages) override;

	virtual NCLMatrix3 buildInverseInertia(float invMass) const override;

	virtual void getCollisionAxes(const PhysicsNode* otherObject, std::vector<NCLVector3>& out_axes) const override;

	virtual NCLVector3 getClosestPoint(const NCLVector3& point) const override;

	virtual void getMinMaxVertexOnAxis(const NCLVector3& axis, NCLVector3& out_min, NCLVector3& out_max) const override;

	virtual void getIncidentReferencePolygon(const NCLVector3& axis, std::list<NCLVector3>& out_face, NCLVector3& out_normal, std::vector<Plane>& out_adjacent_planes) const override;

private:
	float radius;
};

