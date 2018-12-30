#include "SphereCollisionShape.h"

#include "PhysicsNode.h"
#include "../Utilities/Maths/Matrix3.h"
#include "../Utilities/Maths/Vector3.h"

#include "../Communication/Messages/DebugLineMessage.h"
#include "../Communication/Messages/DebugSphereMessage.h"

SphereCollisionShape::SphereCollisionShape()
{
	m_Radius = 1.0f;
}

SphereCollisionShape::SphereCollisionShape(float radius)
{
	m_Radius = radius;
}

SphereCollisionShape::~SphereCollisionShape()
{

}

NCLMatrix3 SphereCollisionShape::BuildInverseInertia(float invMass) const
{
	//https://en.wikipedia.org/wiki/List_of_moments_of_inertia
	float i = 2.5f * invMass / (m_Radius * m_Radius); //SOLID
	//float i = 1.5f * invMass / (m_Radius * m_Radius); //HOLLOW

	NCLMatrix3 inertia;
	inertia._11 = i;
	inertia._22 = i;
	inertia._33 = i;

	return inertia;
}


void SphereCollisionShape::GetCollisionAxes(PhysicsNode* otherObject, std::vector<NCLVector3>& out_axes) const
{
	/* There are infinite possible axes on a sphere so we MUST handle it seperately
		- Luckily we can just get the closest point on the opposite object to our centre and use that.
	*/
	NCLVector3 dir = (otherObject->GetPosition() - Parent()->GetPosition()).normalise();

	NCLVector3 p1 = Parent()->GetPosition();
	NCLVector3 p2(0, 0, 0);
	float dist = FLT_MAX;

	for each (CollisionShape* shape in otherObject->collisionShapes)
	{
		NCLVector3 current = shape->GetClosestPoint(p1);
		float currentDist = (p1 - current).length();
		if (currentDist < dist)
		{
			p2 = current;
			dist = currentDist;
		}
	}

	out_axes.push_back((p1 - p2).normalise());
}

NCLVector3 SphereCollisionShape::GetClosestPoint(const NCLVector3& point) const
{
	NCLVector3 diff = (point - Parent()->GetPosition()).normalise();
	return Parent()->GetPosition() + diff * m_Radius;
}

void SphereCollisionShape::GetMinMaxVertexOnAxis(
	const NCLVector3& axis,
	NCLVector3& out_min,
	NCLVector3& out_max) const
{
	out_min = Parent()->GetPosition() - axis * m_Radius;
	out_max = Parent()->GetPosition() + axis * m_Radius;
}
//-------------

void SphereCollisionShape::GetIncidentReferencePolygon(
	const NCLVector3& axis,
	std::list<NCLVector3>& out_face,
	NCLVector3& out_normal,
	std::vector<Plane>& out_adjacent_planes) const
{
	//This is used in Tutorial 5
	out_face.push_back(Parent()->GetPosition() + axis * m_Radius);
	out_normal = axis;	
}

void SphereCollisionShape::DebugDraw(std::vector<DebugLineMessage>& lineMessages, std::vector<DebugSphereMessage>& sphereMessages) const
{
	NCLVector3 pos = Parent()->GetPosition();

	//Draw Filled Circle
	//NCLDebug::DrawPointNDT(pos, m_Radius, Vector4(1.0f, 1.0f, 1.0f, 0.2f));

	//Draw Perimeter Axes
	NCLVector3 lastX = pos + NCLVector3(0.0f, 1.0f, 0.0f) * m_Radius;
	NCLVector3 lastY = pos + NCLVector3(1.0f, 0.0f, 0.0f) * m_Radius;
	NCLVector3 lastZ = pos + NCLVector3(1.0f, 0.0f, 0.0f) * m_Radius;
	const int nSubdivisions = 20;
	for (int itr = 1; itr <= nSubdivisions; ++itr)
	{
		float angle = itr / float(nSubdivisions) * PI * 2.f;
		float alpha = cosf(angle) * m_Radius;
		float beta = sinf(angle) * m_Radius;

		NCLVector3 newX = pos + NCLVector3(0.0f, alpha, beta);
		NCLVector3 newY = pos + NCLVector3(alpha, 0.0f, beta);
		NCLVector3 newZ = pos + NCLVector3(alpha, beta, 0.0f);

		lineMessages.push_back(DebugLineMessage("RenderingSystem", lastX, newX, NCLVector3(1.0f, 0.3f, 1.0f)));
		lineMessages.push_back(DebugLineMessage("RenderingSystem", lastY, newY, NCLVector3(1.0f, 0.3f, 1.0f)));
		lineMessages.push_back(DebugLineMessage("RenderingSystem", lastZ, newZ, NCLVector3(1.0f, 0.3f, 1.0f)));

		lastX = newX;
		lastY = newY;
		lastZ = newZ;
	}
}