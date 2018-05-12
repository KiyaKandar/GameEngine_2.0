#include "Matrix3.h"
#include "Matrix4.h"

const NCLMatrix3 NCLMatrix3::Identity = NCLMatrix3(1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f);

const NCLMatrix3 NCLMatrix3::ZeroMatrix = NCLMatrix3(0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f);

//ctor
NCLMatrix3::NCLMatrix3() :
	_11(1.0f), _12(0.0f), _13(0.0f),
	_21(0.0f), _22(1.0f), _23(0.0f),
	_31(0.0f), _32(0.0f), _33(1.0f)
{
}

NCLMatrix3::NCLMatrix3(float e[9]) :
	_11(e[0]), _12(e[1]), _13(e[2]),
	_21(e[3]), _22(e[4]), _23(e[5]),
	_31(e[6]), _32(e[7]), _33(e[8])
{
}

NCLMatrix3::NCLMatrix3(const NCLVector3& c1, const NCLVector3& c2, const NCLVector3& c3)
{
	const unsigned int size = 3 * sizeof(float);
	memcpy(&mat_array[0], &c1.x, size);
	memcpy(&mat_array[3], &c2.x, size);
	memcpy(&mat_array[6], &c3.x, size);
}

NCLMatrix3::NCLMatrix3(float a1, float a2, float a3,
	float b1, float b2, float b3,
	float c1, float c2, float c3) :
	_11(a1), _12(a2), _13(a3),
	_21(b1), _22(b2), _23(b3),
	_31(c1), _32(c2), _33(c3)
{
}

NCLMatrix3::NCLMatrix3(const NCLMatrix4& mat44)
{
	const unsigned int size = 3 * sizeof(float);
	memcpy(&mat_array[0], &mat44.values[0], size);
	memcpy(&mat_array[3], &mat44.values[4], size);
	memcpy(&mat_array[6], &mat44.values[8], size);
}

NCLMatrix3::~NCLMatrix3(void)
{
}




//Default States
void NCLMatrix3::toZero()
{
	memset(mat_array, 0, 9 * sizeof(float));
}

void NCLMatrix3::toIdentity()
{
	_11 = 1.0f; _12 = 0.0f; _13 = 0.0f;
	_21 = 0.0f; _22 = 1.0f; _23 = 0.0f;
	_31 = 0.0f; _32 = 0.0f; _33 = 1.0f;
}



//Transformation Matrix
NCLMatrix3 NCLMatrix3::rotation(float degrees, const NCLVector3 &inaxis)
{
	NCLMatrix3 m;

	NCLVector3 axis = inaxis;
	axis.normalise();

	float c = cosf((float)DegToRad(degrees));
	float s = sinf((float)DegToRad(degrees));

	m(0, 0) = (axis.x * axis.x) * (1.0f - c) + c;
	m(1, 0) = (axis.y * axis.x) * (1.0f - c) + (axis.z * s);
	m(2, 0) = (axis.z * axis.x) * (1.0f - c) - (axis.y * s);

	m(0, 1) = (axis.x * axis.y) * (1.0f - c) - (axis.z * s);
	m(1, 1) = (axis.y * axis.y) * (1.0f - c) + c;
	m(2, 1) = (axis.z * axis.y) * (1.0f - c) + (axis.x * s);

	m(0, 2) = (axis.x * axis.z) * (1.0f - c) + (axis.y * s);
	m(1, 2) = (axis.y * axis.z) * (1.0f - c) - (axis.x * s);
	m(2, 2) = (axis.z * axis.z) * (1.0f - c) + c;

	return m;
}

NCLMatrix3 NCLMatrix3::rotation(const NCLVector3 &forward_direction, const NCLVector3& up_direction)
{
	NCLVector3 f = forward_direction;
	NCLVector3 u = up_direction;

	f.normalise();
	u.normalise();

	NCLVector3 x = NCLVector3::cross(f, u); x.normalise();
	NCLVector3 y = NCLVector3::cross(x, f); y.normalise();

	NCLMatrix3 m;

	m(0, 0) = x.x;
	m(1, 0) = x.y;
	m(2, 0) = x.z;

	m(0, 1) = y.x;
	m(1, 1) = y.y;
	m(2, 1) = y.z;

	m(0, 2) = -f.x;
	m(1, 2) = -f.y;
	m(2, 2) = -f.z;

	return m;
}

NCLMatrix3 NCLMatrix3::scale(const NCLVector3 &scale)
{
	NCLMatrix3 m;
	m.setScalingVector(scale);
	return m;
}



// Standard Matrix Functionality
NCLMatrix3 NCLMatrix3::inverse(const NCLMatrix3& rhs)
{
	NCLMatrix3 out;
	float det = rhs.determinant();
	if (det != 0.f)
	{
		float invdet = 1.0f / det;
		out(0, 0) = (rhs(1, 1)*rhs(2, 2) - rhs(2, 1)*rhs(1, 2)) * invdet;
		out(0, 1) = -(rhs(0, 1)*rhs(2, 2) - rhs(0, 2)*rhs(2, 1)) * invdet;
		out(0, 2) = (rhs(0, 1)*rhs(1, 2) - rhs(0, 2)*rhs(1, 1)) * invdet;

		out(1, 0) = -(rhs(1, 0)*rhs(2, 2) - rhs(1, 2)*rhs(2, 0)) * invdet;
		out(1, 1) = (rhs(0, 0)*rhs(2, 2) - rhs(0, 2)*rhs(2, 0)) * invdet;
		out(1, 2) = -(rhs(0, 0)*rhs(1, 2) - rhs(1, 0)*rhs(0, 2)) * invdet;

		out(2, 0) = (rhs(1, 0)*rhs(2, 1) - rhs(2, 0)*rhs(1, 1)) * invdet;
		out(2, 1) = -(rhs(0, 0)*rhs(2, 1) - rhs(2, 0)*rhs(0, 1)) * invdet;
		out(2, 2) = (rhs(0, 0)*rhs(1, 1) - rhs(1, 0)*rhs(0, 1)) * invdet;
	}
	return out;
}

NCLMatrix3 NCLMatrix3::transpose(const NCLMatrix3& rhs)
{
	NCLMatrix3 m;

	m._11 = rhs._11;
	m._21 = rhs._12;
	m._31 = rhs._13;

	m._12 = rhs._21;
	m._22 = rhs._22;
	m._32 = rhs._23;

	m._13 = rhs._31;
	m._23 = rhs._32;
	m._33 = rhs._33;

	return m;
}

NCLMatrix3 NCLMatrix3::adjugate(const NCLMatrix3& m)
{
	NCLMatrix3 adj;

	adj._11 = m._22 * m._33 - m._23 * m._32;
	adj._12 = m._13 * m._32 - m._12 * m._33;
	adj._13 = m._12 * m._23 - m._13 * m._22;

	adj._21 = m._23 * m._31 - m._21 * m._33;
	adj._22 = m._11 * m._33 - m._13 * m._31;
	adj._23 = m._13 * m._21 - m._11 * m._23;

	adj._31 = m._21 * m._32 - m._22 * m._31;
	adj._32 = m._12 * m._31 - m._11 * m._32;
	adj._33 = m._11 * m._22 - m._12 * m._21;

	return adj;
}

NCLMatrix3 NCLMatrix3::outerProduct(const NCLVector3& a, const NCLVector3& b)
{
	NCLMatrix3 m;

	m._11 = a.x * b.x;
	m._12 = a.x * b.y;
	m._13 = a.x * b.z;

	m._21 = a.y * b.x;
	m._22 = a.y * b.y;
	m._23 = a.y * b.z;

	m._31 = a.z * b.x;
	m._32 = a.z * b.y;
	m._33 = a.z * b.z;

	return m;
}



// Additional Functionality
float NCLMatrix3::trace() const
{
	return _11 + _22 + _33;
}

float NCLMatrix3::determinant() const
{
	return _11*(_22*_33 - _32*_23) - _12*(_21*_33 - _23*_31) + _13*(_21*_32 - _22*_31);
}



NCLMatrix3& operator+=(NCLMatrix3& a, const NCLMatrix3& b)
{
	for (unsigned int i = 0; i < 9; ++i)
		a.mat_array[i] += b.mat_array[i];
	return a;
}

NCLMatrix3& operator-=(NCLMatrix3& a, const NCLMatrix3& b)
{
	for (unsigned int i = 0; i < 9; ++i)
		a.mat_array[i] -= b.mat_array[i];
	return a;
}

NCLMatrix3 operator+(const NCLMatrix3& a, const NCLMatrix3& b)
{
	NCLMatrix3 m;
	for (unsigned int i = 0; i < 9; ++i)
		m.mat_array[i] = a.mat_array[i] + b.mat_array[i];
	return m;
}

NCLMatrix3 operator-(const NCLMatrix3& a, const NCLMatrix3& b)
{
	NCLMatrix3 m;
	for (unsigned int i = 0; i < 9; ++i)
		m.mat_array[i] = a.mat_array[i] - b.mat_array[i];
	return m;
}

NCLMatrix3 operator*(const NCLMatrix3& a, const NCLMatrix3& b)
{
	NCLMatrix3 out;

	out._11 = a._11 * b._11 + a._12 * b._21 + a._13 * b._31;
	out._12 = a._11 * b._12 + a._12 * b._22 + a._13 * b._32;
	out._13 = a._11 * b._13 + a._12 * b._23 + a._13 * b._33;

	out._21 = a._21 * b._11 + a._22 * b._21 + a._23 * b._31;
	out._22 = a._21 * b._12 + a._22 * b._22 + a._23 * b._32;
	out._23 = a._21 * b._13 + a._22 * b._23 + a._23 * b._33;

	out._31 = a._31 * b._11 + a._32 * b._21 + a._33 * b._31;
	out._32 = a._31 * b._12 + a._32 * b._22 + a._33 * b._32;
	out._33 = a._31 * b._13 + a._32 * b._23 + a._33 * b._33;

	return out;
}

NCLMatrix3& operator+=(NCLMatrix3& a, const float b)
{
	for (unsigned int i = 0; i < 9; ++i)
		a.mat_array[i] += b;
	return a;
}

NCLMatrix3& operator-=(NCLMatrix3& a, const float b)
{
	for (unsigned int i = 0; i < 9; ++i)
		a.mat_array[i] -= b;
	return a;
}
NCLMatrix3& operator*=(NCLMatrix3& a, const float b)
{
	for (unsigned int i = 0; i < 9; ++i)
		a.mat_array[i] *= b;
	return a;
}
NCLMatrix3& operator/=(NCLMatrix3& a, const float b)
{
	for (unsigned int i = 0; i < 9; ++i)
		a.mat_array[i] /= b;
	return a;
}

NCLMatrix3 operator+(NCLMatrix3& a, const float b)
{
	NCLMatrix3 m;
	for (unsigned int i = 0; i < 9; ++i)
		m.mat_array[i] = a.mat_array[i] + b;
	return m;
}

NCLMatrix3 operator-(const NCLMatrix3& a, const float b)
{
	NCLMatrix3 m;
	for (unsigned int i = 0; i < 9; ++i)
		m.mat_array[i] = a.mat_array[i] - b;
	return m;
}
NCLMatrix3 operator*(const NCLMatrix3& a, const float b)
{
	NCLMatrix3 m;
	for (unsigned int i = 0; i < 9; ++i)
		m.mat_array[i] = a.mat_array[i] * b;
	return m;
}
NCLMatrix3 operator/(const NCLMatrix3& a, const float b)
{
	NCLMatrix3 m;
	for (unsigned int i = 0; i < 9; ++i)
		m.mat_array[i] = a.mat_array[i] / b;
	return m;
}

NCLVector3 operator*(const NCLMatrix3& a, const NCLVector3& b)
{
	NCLVector3 out;

	out.x = a._11 * b.x
		+ a._21 * b.y
		+ a._31 * b.z;

	out.y = a._12 * b.x
		+ a._22 * b.y
		+ a._32 * b.z;

	out.z = a._13 * b.x
		+ a._23 * b.y
		+ a._33 * b.z;

	return out;
}
