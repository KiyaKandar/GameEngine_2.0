#include "CuboidCollisionShape.h"
#include "PhysicsNode.h"
#include "GeometryUtils.h"
#include "../Utilities/Maths/Matrix3.h"

Hull CuboidCollisionShape::cubeHull = Hull();

CuboidCollisionShape::CuboidCollisionShape()
{
	halfDims = NCLVector3(0.5f, 0.5f, 0.5f);
	if (cubeHull.GetNumVertices() == 0)
	{
		constructCubeHull();
	}
}

CuboidCollisionShape::CuboidCollisionShape(const NCLVector3 & halfdims)
{
	halfDims = halfdims;
	if (cubeHull.GetNumVertices() == 0)
	{
		constructCubeHull();
	}
}


CuboidCollisionShape::~CuboidCollisionShape()
{
}

NCLMatrix3 CuboidCollisionShape::buildInverseInertia(float invMass) const
{
	NCLMatrix3 inertia;

	NCLVector3 dimsSq = (halfDims + halfDims);
	dimsSq = dimsSq * dimsSq;

	inertia._11 = 12.f * invMass / (dimsSq.y + dimsSq.z);
	inertia._22 = 12.f * invMass / (dimsSq.x + dimsSq.z);
	inertia._33 = 12.f * invMass / (dimsSq.x + dimsSq.y);

	return inertia;
}

void CuboidCollisionShape::getCollisionAxes(const PhysicsNode * otherObject, std::vector<NCLVector3>& out_axes) const
{
	NCLMatrix3 objOrientation = parent()->getOrientation().toMatrix3();
	out_axes.push_back(objOrientation * NCLVector3(1.0f, 0.0f, 0.0f)); //X - Axis
	out_axes.push_back(objOrientation * NCLVector3(0.0f, 1.0f, 0.0f)); //Y - Axis
	out_axes.push_back(objOrientation * NCLVector3(0.0f, 0.0f, 1.0f)); //Z - Axis
}

NCLVector3 CuboidCollisionShape::getClosestPoint(const NCLVector3 & point) const
{
	
	NCLMatrix4 wsTransform = parent()->getWorldSpaceTransform() * NCLMatrix4::scale(halfDims);

	NCLMatrix4 invWsTransform = NCLMatrix4::Inverse(wsTransform);
	NCLVector3 local_point = invWsTransform * point;

	float out_distSq = FLT_MAX;
	NCLVector3 out_point;
	for (size_t i = 0; i < cubeHull.GetNumEdges(); ++i)
	{
		const HullEdge& e = cubeHull.GetEdge(i);
		NCLVector3 start = cubeHull.GetVertex(e._vStart)._pos;
		NCLVector3 end = cubeHull.GetVertex(e._vEnd)._pos;

		NCLVector3 ep = GeometryUtils::getClosestPoint(local_point, GeometryUtils::Edge(start, end));

		float distSq = NCLVector3::dot(ep - local_point, ep - local_point);
		if (distSq < out_distSq)
		{
			out_distSq = distSq;
			out_point = ep;
		}
	}

	return wsTransform * out_point;
}

void CuboidCollisionShape::getMinMaxVertexOnAxis(const NCLVector3 & axis, NCLVector3 & out_min, NCLVector3 & out_max) const
{
	NCLMatrix4 wsTransform = parent()->getWorldSpaceTransform() * NCLMatrix4::scale(halfDims);

	NCLMatrix3 invNormalMatrix = NCLMatrix3::inverse(NCLMatrix3(wsTransform));
	NCLVector3 local_axis = invNormalMatrix * axis;
	local_axis.normalise();

	int vMin, vMax;
	cubeHull.GetMinMaxVerticesInAxis(local_axis, &vMin, &vMax);

	out_min = wsTransform * cubeHull.GetVertex(vMin)._pos;
	out_max = wsTransform * cubeHull.GetVertex(vMax)._pos;
}

void CuboidCollisionShape::getIncidentReferencePolygon(const NCLVector3 & axis, std::list<NCLVector3>& out_face, NCLVector3 & out_normal, std::vector<Plane>& out_adjacent_planes) const
{
	NCLMatrix4 wsTransform = parent()->getWorldSpaceTransform() * NCLMatrix4::scale(halfDims);

	NCLMatrix3 invNormalMatrix = NCLMatrix3::inverse(NCLMatrix3(wsTransform));
	NCLMatrix3 normalMatrix = NCLMatrix3::inverse(invNormalMatrix);

	NCLVector3 local_axis = invNormalMatrix * axis;

	int undefined, maxVertex;
	cubeHull.GetMinMaxVerticesInAxis(local_axis, &undefined, &maxVertex);
	const HullVertex& vert = cubeHull.GetVertex(maxVertex);

	const HullFace* best_face = 0;
	float best_correlation = -FLT_MAX;
	for (int faceIdx : vert._enclosing_faces)
	{
		const HullFace* face = &cubeHull.GetFace(faceIdx);
		float temp_correlation = NCLVector3::dot(local_axis, face->_normal);
		if (temp_correlation > best_correlation)
		{
			best_correlation = temp_correlation;
			best_face = face;
		}
	}

	out_normal = (normalMatrix * best_face->_normal).normalise();

	for (int vertIdx : best_face->_vert_ids)
	{
		const HullVertex& vert = cubeHull.GetVertex(vertIdx);
		out_face.push_back(wsTransform * vert._pos);
	}

	NCLVector3 wsPointOnPlane = wsTransform * cubeHull.GetVertex(cubeHull.GetEdge(best_face->_edge_ids[0])._vStart)._pos;

	{
		NCLVector3 planeNrml = -(normalMatrix * best_face->_normal);
		planeNrml.normalise();

		float planeDist = -NCLVector3::dot(planeNrml, wsPointOnPlane);
	}

	for (int edgeIdx : best_face->_edge_ids)
	{
		const HullEdge& edge = cubeHull.GetEdge(edgeIdx);

		wsPointOnPlane = wsTransform * cubeHull.GetVertex(edge._vStart)._pos;

		for (int adjFaceIdx : edge._enclosing_faces)
		{
			if (adjFaceIdx != best_face->_idx)
			{
				const HullFace& adjFace = cubeHull.GetFace(adjFaceIdx);

				NCLVector3 planeNrml = -(normalMatrix * adjFace._normal);
				planeNrml.normalise();
				float planeDist = -NCLVector3::dot(planeNrml, wsPointOnPlane);

				out_adjacent_planes.push_back(Plane(planeNrml, planeDist));
			}
		}
	}
}

void CuboidCollisionShape::constructCubeHull()
{
	//Vertices
	cubeHull.AddVertex(NCLVector3(-1.0f, -1.0f, -1.0f));		// 0
	cubeHull.AddVertex(NCLVector3(-1.0f, 1.0f, -1.0f));		// 1
	cubeHull.AddVertex(NCLVector3(1.0f, 1.0f, -1.0f));		// 2
	cubeHull.AddVertex(NCLVector3(1.0f, -1.0f, -1.0f));		// 3

	cubeHull.AddVertex(NCLVector3(-1.0f, -1.0f, 1.0f));		// 4
	cubeHull.AddVertex(NCLVector3(-1.0f, 1.0f, 1.0f));		// 5
	cubeHull.AddVertex(NCLVector3(1.0f, 1.0f, 1.0f));		// 6
	cubeHull.AddVertex(NCLVector3(1.0f, -1.0f, 1.0f));		// 7

														//Indices ( MUST be provided in ccw winding order )
	int face1[] = { 0, 1, 2, 3 };
	int face2[] = { 7, 6, 5, 4 };
	int face3[] = { 5, 6, 2, 1 };
	int face4[] = { 0, 3, 7, 4 };
	int face5[] = { 6, 7, 3, 2 };
	int face6[] = { 4, 5, 1, 0 };

	//Faces
	cubeHull.AddFace(NCLVector3(0.0f, 0.0f, -1.0f), 4, face1);
	cubeHull.AddFace(NCLVector3(0.0f, 0.0f, 1.0f), 4, face2);
	cubeHull.AddFace(NCLVector3(0.0f, 1.0f, 0.0f), 4, face3);
	cubeHull.AddFace(NCLVector3(0.0f, -1.0f, 0.0f), 4, face4);
	cubeHull.AddFace(NCLVector3(1.0f, 0.0f, 0.0f), 4, face5);
	cubeHull.AddFace(NCLVector3(-1.0f, 0.0f, 0.0f), 4, face6);
}

void CuboidCollisionShape::debugDraw()
{
	// Just draw the cuboid hull-mesh at the position of our PhysicsNode
	NCLMatrix4 transform = this->parent()->getWorldSpaceTransform() * NCLMatrix4::scale(halfDims);
	cubeHull.DebugDraw(transform);
}