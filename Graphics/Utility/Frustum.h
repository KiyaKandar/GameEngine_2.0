#pragma once

#include "../Utilities/Maths/Plane.h"
#include "../Utilities/Maths/Matrix4.h"

class NCLMatrix4;

class Frustum
{
public:
	Frustum() {}
	~Frustum() {}

	void fromMatrix(const NCLMatrix4 &mvp);
	bool insideFrustum(const NCLVector3& position, const float& radius);

protected:
	Plane planes[6];
};