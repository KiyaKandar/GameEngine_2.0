#pragma once

#include "Vector3.h"

class NCLVector4
{
public:
	NCLVector4(void)
	{
		x = y = z = w = 1.0f;
	}

	NCLVector4(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	NCLVector3 ToVector3() const
	{
		return NCLVector3(x, y, z);
	}

	~NCLVector4(void)
	{
	}

	float x;
	float y;
	float z;
	float w;

	inline NCLVector4 operator-(const NCLVector4& a) const
	{
		return NCLVector4(x - a.x, y - a.y, z - a.z, w - a.w);
	}
};
