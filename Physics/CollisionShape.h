#pragma once

//#include "GeometryUtils.h"
#include "../Utilities/Maths/Vector3.h"
#include "../Utilities/Maths/Plane.h"
#include "../Utilities/Maths/Matrix3.h"
#include <vector>
#include <list>

//using namespace GeometryUtils;

class PhysicsNode;

struct CollisionEdge
{
	CollisionEdge(const NCLVector3& a, const NCLVector3& b)
		: _v0(a), _v1(b) {}

	NCLVector3 _v0;
	NCLVector3 _v1;
};

class CollisionShape
{
public:
	CollisionShape() : m_Parent(NULL) {}
	virtual ~CollisionShape() {}

	virtual NCLMatrix3 buildInverseInertia(float invMass) const = 0;
	virtual void debugDraw() = 0;

	inline void setParent(PhysicsNode* node) 
	{ 
		m_Parent = node; 
	}
	inline		 PhysicsNode* parent() 
	{ 
		return m_Parent; 
	}
	inline const PhysicsNode* parent() const 
	{ 
		return m_Parent; 
	}

	virtual void setScale(NCLVector3 scale, float invMass) = 0;

	virtual void getCollisionAxes(
		const PhysicsNode* otherObject,
		std::vector<NCLVector3>& out_axes) const = 0;

	virtual NCLVector3 getClosestPoint(const NCLVector3& point) const = 0;

	virtual void getMinMaxVertexOnAxis(
		const NCLVector3& axis,
		NCLVector3& out_min,
		NCLVector3& out_max) const = 0;

	virtual void getIncidentReferencePolygon(
		const NCLVector3& axis,
		std::list<NCLVector3>& out_face,
		NCLVector3& out_normal,
		std::vector<Plane>& out_adjacent_planes) const = 0;

protected:
	PhysicsNode* m_Parent;
};