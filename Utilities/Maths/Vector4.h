/*
Class:Vector4
Implements:
Author:Rich Davison
Description:VERY simple Vector4 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#pragma once

#include "Vector3.h"

class NCLVector4	{
public:
	NCLVector4(void) {
		x = y = z = w = 1.0f;
	}
	NCLVector4(float x, float y, float z, float w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	NCLVector3 toVector3() {
		return NCLVector3(x, y, z);
	}

	~NCLVector4(void){}

	float x;
	float y;
	float z;
	float w;

	inline NCLVector4  operator-(const NCLVector4  &a) const
	{
		return NCLVector4(x - a.x, y - a.y, z - a.z, w - a.w);
	}
};

