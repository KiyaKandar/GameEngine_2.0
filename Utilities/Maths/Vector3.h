#pragma once
/*
Class:Vector3
Implements:
Author:Rich Davison
Description:VERY simple Vector3 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#include <cmath>
#include <iostream>
#include "../../Resource Management/XMLParser.h"
#include "Vector2.h"
#include "VectorBuilder.h"
#include <iterator>
#include <sstream>

#include <vector3.h>

class NCLVector3
{
public:
	NCLVector3(void)
	{
		ToZero();
	}

	NCLVector3(const float x, const float y, const float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	NCLVector3(NCLVector2 v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = 1;
	}

	NCLVector3(const aiVector3D& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
	}

	~NCLVector3(void)
	{
	}

	float x;
	float y;
	float z;

	static NCLVector3 Interpolate(const NCLVector3& a, const NCLVector3& b, float factor)
	{
		return a + ((b - a) * factor);
	}

	const NCLVector3& Normalise()
	{
		float currentLength = Length();

		if (currentLength != 0.0f)
		{
			currentLength = 1.0f / currentLength;
			x = x * currentLength;
			y = y * currentLength;
			z = z * currentLength;
		}

		return *this;
	}

	float sqrLength() const
	{
		return (x * x) + (y * y) + (z * z);
	}

	void ToZero()
	{
		x = y = z = 0.0f;
	}

	float Length() const
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}

	float LengthSquared() const
	{
		return (x * x + y * y + z * z);
	}

	void Invert()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	NCLVector3 Inverse() const
	{
		return NCLVector3(-x, -y, -z);
	}

	static float dot(const NCLVector3& a, const NCLVector3& b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	}

	static NCLVector3 Cross(const NCLVector3& a, const NCLVector3& b)
	{
		return NCLVector3((a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x));
	}

	inline friend std::ostream& operator<<(std::ostream& o, const NCLVector3& v)
	{
		o << "Vector3(" << v.x << "," << v.y << "," << v.z << ")" << std::endl;
		return o;
	}

	inline NCLVector3 operator+(const NCLVector3& a) const
	{
		return NCLVector3(x + a.x, y + a.y, z + a.z);
	}

	inline NCLVector3 operator-(const NCLVector3& a) const
	{
		return NCLVector3(x - a.x, y - a.y, z - a.z);
	}

	inline NCLVector3 operator-() const
	{
		return NCLVector3(-x, -y, -z);
	}

	inline void operator+=(const NCLVector3& a)
	{
		x += a.x;
		y += a.y;
		z += a.z;
	}

	inline void operator-=(const NCLVector3& a)
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;
	}

	inline NCLVector3 operator*(const float a) const
	{
		return NCLVector3(x * a, y * a, z * a);
	}

	inline NCLVector3 operator*(const NCLVector3& a) const
	{
		return NCLVector3(x * a.x, y * a.y, z * a.z);
	}

	inline NCLVector3 operator/(const NCLVector3& a) const
	{
		return NCLVector3(x / a.x, y / a.y, z / a.z);
	};

	inline NCLVector3 operator/(const float v) const
	{
		return NCLVector3(x / v, y / v, z / v);
	};

	inline bool operator==(const NCLVector3& A) const
	{
		return (A.x == x && A.y == y && A.z == z) ? true : false;
	};

	inline bool operator!=(const NCLVector3& A) const
	{
		return (A.x == x && A.y == y && A.z == z) ? false : true;
	};
};
