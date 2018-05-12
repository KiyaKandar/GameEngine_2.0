#pragma once

#include "../Utilities/Maths/Vector3.h"

struct AABB
{
	NCLVector3 min;
	NCLVector3 max;
};

static bool BoxesIntersect(const AABB& boxA, const AABB& boxB)
{
	return 
		(boxA.min.x <= boxB.max.x && boxA.max.x >= boxB.min.x) &&
		(boxA.min.y <= boxB.max.y && boxA.max.y >= boxB.min.y) &&
		(boxA.min.z <= boxB.max.z && boxA.max.z >= boxB.min.z);
}