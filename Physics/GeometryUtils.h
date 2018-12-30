#pragma once

#include "../Utilities/Maths/Vector3.h"
#include "../Utilities/Maths/Plane.h"
#include <list>
#include <vector>

namespace GeometryUtils
{
	struct Edge
	{
		Edge() : _v0(0.0f, 0.0f, 0.0f) , _v1(0.0f, 0.0f, 0.0f) {}
		Edge(NCLVector3 start, NCLVector3 end) : _v0(start) , _v1(end) {}


		NCLVector3 _v0;
		NCLVector3 _v1;
	};

	// Gets the closest point x on the line (edge) to point (pos)
	NCLVector3 GetClosestPoint(
		const NCLVector3& pos,
		const Edge& edge);

	// Iterates through all edges in polygon (defined as a line-loop list
	// of vertices) and returns the closest point X to point (pos) that 
	// resides on any of the given edges of the polygon.
	NCLVector3 GetClosestPointPolygon(
		const NCLVector3& pos,
		const std::list<NCLVector3>& polygon);

	// Iterates through all edges returning the the point X which is the closest
	//   point along any of the given edges to the provided point A as possible.
	NCLVector3 GetClosestPoint(
		const NCLVector3& pos,
		std::vector<Edge>& edges);


	// Performs a plane/edge collision test, if an intersection does occur then
	//    it will return the point on the line where it intersected the given plane.
	bool PlaneEdgeIntersection(
		const Plane& plane,
		const NCLVector3& start,
		const NCLVector3& end,
		NCLVector3& out_point);

	//Performs sutherland hodgman clipping algorithm to clip the provided polygon
	// in regards to each of the provided clipping planes.
	// https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm
	void SutherlandHodgmanClipping(
		const std::list<NCLVector3>& input_polygon,
		int num_clip_planes,
		const Plane* clip_planes,
		std::list<NCLVector3>* out_polygon,
		bool removeNotClipToPlane);
};