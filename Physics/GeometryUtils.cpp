#include "GeometryUtils.h"
#include "../Utilities/Maths/MathsCommon.h"

// Gets the closest point x on the line (edge) to point (pos)
NCLVector3 GeometryUtils::getClosestPoint(
	const NCLVector3& pos,
	const Edge& edge)
{
	//As we have a line not two points, the final value could be anywhere between the edge points A/B
	//	We solve this by projecting the point (pos) onto the line described by the edge, and then
	//  clamping it so can it has to be between the line's start and end points.
	//  - Notation: A - Line Start, B - Line End, P - Point not on the line we want to project
	NCLVector3 diff_AP = pos - edge._v0;
	NCLVector3 diff_AB = edge._v1 - edge._v0;

	//Distance along the line of point 'pos' in world distance 
	float ABAPproduct = NCLVector3::dot(diff_AP, diff_AB);
	float magnitudeAB = NCLVector3::dot(diff_AB, diff_AB);

	//Distance along the line of point 'pos' between 0-1 where 0 is line start and 1 is line end
	float distance = ABAPproduct / magnitudeAB;

	//Clamp distance so it cant go beyond the line's start/end in either direction
	distance = max(min(distance, 1.0f), 0.0f);

	//Use distance from line start (0-1) to compute the actual position
	return edge._v0 + diff_AB * distance;
}





// Iterates through all edges in polygon (defined as a line-loop list
// of vertices) and returns the closest point X to point (pos) that 
// resides on any of the given edges of the polygon.
NCLVector3 GeometryUtils::getClosestPointPolygon(
	const NCLVector3& pos,
	const std::list<NCLVector3>& polygon)
{
	NCLVector3 final_closest_point = NCLVector3(0.0f, 0.0f, 0.0f);
	float final_closest_distsq = FLT_MAX;

	NCLVector3 last = polygon.back();
	for (const NCLVector3& next : polygon)
	{

		NCLVector3 edge_closest_point = getClosestPoint(pos, GeometryUtils::Edge(last, next));

		//Compute the distance of the closest point on the line to the actual point
		NCLVector3 diff = pos - edge_closest_point;
		float temp_distsq = NCLVector3::dot(diff, diff);

		//Only store the closest point if it's closer than all the other line's closest points
		if (temp_distsq < final_closest_distsq)
		{
			final_closest_distsq = temp_distsq;
			final_closest_point = edge_closest_point;
		}

		last = next;
	}

	return final_closest_point;
}

// Iterates through all edges returning the the point X which is the closest
//   point along any of the given edges to the provided point A as possible.
NCLVector3 GeometryUtils::getClosestPoint(
	const NCLVector3& pos,
	std::vector<GeometryUtils::Edge>& edges)
{
	NCLVector3 final_closest_point = NCLVector3(0.0f, 0.0f, 0.0f);
	float final_closest_distsq = FLT_MAX;

	for (const GeometryUtils::Edge& edge : edges)
	{
		NCLVector3 edge_closest_point = getClosestPoint(pos, edge);

		//Compute the distance of the closest point on the line to the actual point
		NCLVector3 diff = pos - edge_closest_point;
		float temp_distsq = NCLVector3::dot(diff, diff);

		//Only store the closest point if it's closer than all the other line's closest points
		if (temp_distsq < final_closest_distsq)
		{
			final_closest_distsq = temp_distsq;
			final_closest_point = edge_closest_point;
		}
	}

	return final_closest_point;
}

// Performs a plane/edge collision test, if an intersection does occur then
//    it will return the point on the line where it intersected the given plane.
bool GeometryUtils::planeEdgeIntersection(
	const Plane& plane,
	const NCLVector3& start,
	const NCLVector3& end,
	NCLVector3& out_point)
{
	NCLVector3 ab = end - start;

	//Check that the edge and plane are not parallel and thus never intersect
	// We do this by projecting the line (start - A, End - B) ab along the plane
	float ab_p = NCLVector3::dot(plane.getNormal(), ab);
	if (fabs(ab_p) > 1e-6f)
	{
		//Generate a random point on the plane (any point on the plane will suffice)
		NCLVector3 p_co = plane.getNormal() * (-plane.getDistance());

		//Work out the edge factor to scale edge by
		// e.g. how far along the edge to traverse before it meets the plane.
		//      This is computed by: -proj<plane_nrml>(edge_start - any_planar_point) / proj<plane_nrml>(edge_start - edge_end)
		float fac = -NCLVector3::dot(plane.getNormal(), start - p_co) / ab_p;

		//Stop any large floating point divide issues with almost parallel planes
		fac = min(max(fac, 0.0f), 1.0f);

		//Return point on edge
		out_point = start + ab * fac;
		return true;
	}

	return false;
}

//Performs sutherland hodgman clipping algorithm to clip the provided mesh
//    or polygon in regards to each of the provided clipping planes.
void GeometryUtils::sutherlandHodgmanClipping(
	const std::list<NCLVector3>& input_polygon,
	int num_clip_planes,
	const Plane* clip_planes,
	std::list<NCLVector3>* out_polygon,
	bool removeNotClipToPlane)
{
	if (!out_polygon)
		return;

	if (num_clip_planes == 0)
		*out_polygon = input_polygon;

	//Create temporary list of vertices
	// - We will keep ping-pong'ing between the two lists updating them as we go.
	std::list<NCLVector3> ppPolygon1, ppPolygon2;
	std::list<NCLVector3> *input = &ppPolygon1, *output = &ppPolygon2;

	*input = input_polygon;


	//Iterate over each clip_plane provided
	for (int i = 0; i < num_clip_planes; ++i)
	{
		//If every single point on our shape has already been removed previously, just exit
		if (input->empty())
			break;

		const Plane& plane = clip_planes[i];

		//Loop through each edge of the polygon (see line_loop from gfx) and clip
		// that edge against the current plane.
		NCLVector3 tempPoint, startPoint = input->back();
		for (const NCLVector3& endPoint : *input)
		{
			bool startInPlane = plane.pointInPlane(startPoint);
			bool endInPlane = plane.pointInPlane(endPoint);

			//If it's the final pass, just remove all points outside the reference plane
			// - This won't return a true polygon if set, but is needed for the last step
			//   we do in the manifold generation
			if (removeNotClipToPlane)
			{
				if (endInPlane)
					output->push_back(endPoint);
			}
			else
			{
				//If the edge is entirely within the clipping plane, keep it as it is
				if (startInPlane && endInPlane)
				{
					output->push_back(endPoint);
				}
				//If the edge interesects the clipping plane, cut the edge along clip plane
				else if (startInPlane && !endInPlane)
				{
					if (planeEdgeIntersection(plane, startPoint, endPoint, tempPoint))
						output->push_back(tempPoint);
				}
				else if (!startInPlane && endInPlane)
				{
					if (planeEdgeIntersection(plane, startPoint, endPoint, tempPoint))
						output->push_back(tempPoint);

					output->push_back(endPoint);
				}
			}
			//..otherwise the edge is entirely outside the clipping plane and should be removed/ignored

			startPoint = endPoint;
		}

		//Swap input/output polygons, and clear output list for us to generate afresh
		std::swap(input, output);
		output->clear();
	}

	*out_polygon = *input;
}