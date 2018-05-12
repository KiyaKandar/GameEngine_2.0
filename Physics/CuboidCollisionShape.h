#pragma once
#include "CollisionShape.h"
#include "Hull.h"

class CuboidCollisionShape :
	public CollisionShape
{
public:
	CuboidCollisionShape();
	CuboidCollisionShape(const NCLVector3& halfdims);
	~CuboidCollisionShape();

	void setScale(NCLVector3 scale, float invMass) override
	{
		halfDims = scale;
		buildInverseInertia(invMass);
	}

	void setHalfWidth(float half_width) 
	{ 
		halfDims.x = fabs(half_width); 
	}
	void setHalfHeight(float half_height) 
	{ 
		halfDims.y = fabs(half_height); 
	}
	void setHalfDepth(float half_depth) 
	{ 
		halfDims.z = fabs(half_depth); 
	}

	const NCLVector3& getHalfDims() const 
	{ 
		return halfDims; 
	}
	float getHalfWidth()	const 
	{ 
		return halfDims.x; 
	}
	float getHalfHeight()	const 
	{ 
		return halfDims.y; 
	}
	float getHalfDepth()	const 
	{ 
		return halfDims.z; 
	}

	static Hull getcubeHull() { return cubeHull; };


	virtual NCLMatrix3 buildInverseInertia(float invMass) const override;

	virtual void getCollisionAxes(const PhysicsNode* otherObject, std::vector<NCLVector3>& out_axes) const override;

	virtual NCLVector3 getClosestPoint(const NCLVector3& point) const override;

	virtual void getMinMaxVertexOnAxis(const NCLVector3& axis, NCLVector3& out_min, NCLVector3& out_max) const override;

	virtual void getIncidentReferencePolygon(const NCLVector3& axis, std::list<NCLVector3>& out_face, NCLVector3& out_normal, std::vector<Plane>& out_adjacent_planes) const override;
	void debugDraw() override;

private:
	static void constructCubeHull();

	NCLVector3 halfDims;
	static Hull cubeHull;
};

