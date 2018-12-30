/*
	A Generic template for all the functionality needed to represent a convex collision shape. 

	This will be the only thing in the physics engine that defines the geometric shape of the
	attached PhysicsNode. It provides a means for computing the interia tensor (rotational mass)
	and a means to calculate collisions with other unknown collision shapes via CollisionDetectionSAT.

	For example usage, see SphereCollisionShape (implicit shape defined by an algorithm - in this case bounding radius)
	and CuboidCollisionShape (physical shape defined by a set of vertices/faces)

*/

#pragma once

#include "Hull.h"
#include "GeometryUtils.h"
#include "../Utilities/Maths/Vector3.h"
#include "../Utilities/Maths/Plane.h"
#include "../Utilities/Maths/Matrix3.h"
#include <vector>
#include <list>

using namespace GeometryUtils;

class PhysicsNode;
class DebugLineMessage;
class DebugSphereMessage;

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
	virtual ~CollisionShape()	{}

	// Constructs an inverse inertia matrix of the given collision volume. This is the equivilant of the inverse mass of an object for rotation,
	//   a good source for non-inverse inertia matricies can be found here: https://en.wikipedia.org/wiki/List_of_moments_of_inertia
	virtual NCLMatrix3 BuildInverseInertia(float invMass) const = 0;

	// Draws this collision shape to the debug renderer
	virtual void DebugDraw(std::vector<DebugLineMessage>& lineMessages, std::vector<DebugSphereMessage>& sphereMessages) const = 0;

	inline void SetParent(PhysicsNode* node) { m_Parent = node; }
	inline		 PhysicsNode* Parent()		 { return m_Parent; }
	inline const PhysicsNode* Parent() const { return m_Parent; }

	virtual void SetScale(NCLVector3 scale, float invMass) = 0;

//<----- USED BY COLLISION DETECTION ----->
	// Get all collision axes between the current shape and the given
	// object to be tested. The axes returned are just for the shape in question
	// the other shape will also have it's own axes to test.
	virtual void GetCollisionAxes(
		PhysicsNode* otherObject,
		std::vector<NCLVector3>& out_axes) const = 0;

	//Returns closest point on the collision shape to the given point
	virtual NCLVector3 GetClosestPoint(const NCLVector3& point) const = 0;

	// Get the min/max vertices along a given axis
	virtual void GetMinMaxVertexOnAxis(
		const NCLVector3& axis,
		NCLVector3& out_min,
		NCLVector3& out_max) const = 0;


	// Get all data needed to build manifold
	//	- Computes the face that is closest to parallel to that of the given axis,
	//    returning the face (as a list of vertices), face normal and the planes
	//    of all adjacent faces in order to clip against.
	virtual void GetIncidentReferencePolygon(
		const NCLVector3& axis,
		std::list<NCLVector3>& out_face,
		NCLVector3& out_normal,
		std::vector<Plane>& out_adjacent_planes) const = 0;

	NCLVector3 SASMin;
	NCLVector3 SASMax;

protected:
	PhysicsNode* m_Parent;
};