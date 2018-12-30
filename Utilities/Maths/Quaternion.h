#pragma once

#include <iostream>
#include "MathsCommon.h"
#include "Matrix4.h"
#include "Matrix3.h"

class NCLMatrix4;

class Quaternion
{
public:
	Quaternion(void);
	Quaternion(const NCLVector3& vec, float w);
	Quaternion(float x, float y, float z, float w);

	~Quaternion(void);

	float x;
	float y;
	float z;
	float w;

	void Normalise();
	NCLMatrix4 ToMatrix() const;
	NCLMatrix3 ToMatrix3() const;
	NCLMatrix4 ToMatrix4() const;

	Quaternion Conjugate() const;
	void GenerateW(); //builds 4th component when loading in shortened, 3 component quaternions

	static Quaternion EulerAnglesToQuaternion(float pitch, float yaw, float roll);
	static Quaternion AxisAngleToQuaterion(const NCLVector3& vector, float degrees);
	static NCLVector4 QuaternionToAxisAngle(Quaternion quaternion);

	static void RotatePointByQuaternion(const Quaternion& q, NCLVector3& point);

	static Quaternion FromMatrix(const NCLMatrix4& m);

	static float Dot(const Quaternion& a, const Quaternion& b);

	Quaternion operator *(const Quaternion& a) const;
	Quaternion operator *(const NCLVector3& a) const;

	Quaternion operator+(const Quaternion& a) const
	{
		return Quaternion(x + a.x, y + a.y, z + a.z, w + a.w);
	}

	inline friend std::ostream& operator<<(std::ostream& o, const Quaternion& q)
	{
		o << "Quat(" << q.x << "," << q.y << "," << q.z << "," << q.w << ")" << std::endl;
		return o;
	}
};
