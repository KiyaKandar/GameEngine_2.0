#include "Frustum.h"

bool Frustum::insideFrustum(const NCLVector3& position, const float& radius)
{
	for (int p = 0; p < 6; ++p)
	{
		if (!planes[p].sphereInPlane(position, radius))
		{
			return false;
		}
	}

	return true;
}

void Frustum::fromMatrix(const NCLMatrix4 &mat)
{
	NCLVector3 xaxis = NCLVector3(mat.values[0], mat.values[4], mat.values[8]);
	NCLVector3 yaxis = NCLVector3(mat.values[1], mat.values[5], mat.values[9]);
	NCLVector3 zaxis = NCLVector3(mat.values[2], mat.values[6], mat.values[10]);
	NCLVector3 waxis = NCLVector3(mat.values[3], mat.values[7], mat.values[11]);

	//RIGHT
	planes[0] = Plane(waxis - xaxis, (mat.values[15] - mat.values[12]), true);

	// LEFT
	planes[1] = Plane(waxis + xaxis, (mat.values[15] + mat.values[12]), true);

	// BOTTOM
	planes[2] = Plane(waxis + yaxis, (mat.values[15] + mat.values[13]), true);

	// TOP
	planes[3] = Plane(waxis - yaxis, (mat.values[15] - mat.values[13]), true);

	// FAR
	planes[4] = Plane(waxis - zaxis, (mat.values[15] - mat.values[14]), true);

	// NEAR
	planes[5] = Plane(waxis + zaxis, (mat.values[15] + mat.values[14]), true);
}