/******************************************************************************
Class: Matrix3
Implements:
Author: Pieran Marris <p.marris@newcastle.ac.uk>
Description:

A slightly more complete Matrix3 implementation. should provide enough functionality
for the Game Technologies section of the course. It is still by no means fast or
optimal however, so you should still be thinking about how it could be made better.

*//////////////////////////////////////////////////////////////////////////////

#pragma once


#include "Vector3.h"

class NCLMatrix4;

class NCLMatrix3
{
public:

	static const NCLMatrix3 Identity;
	static const NCLMatrix3 ZeroMatrix;

	//ctor
	NCLMatrix3();

	NCLMatrix3(float elements[9]);

	NCLMatrix3(const NCLVector3& c1, const NCLVector3& c2, const NCLVector3& c3);

	NCLMatrix3(float a1, float a2, float a3,
		float b1, float b2, float b3,
		float c1, float c2, float c3);

	NCLMatrix3(const NCLMatrix4& mat44);

	~NCLMatrix3(void);




	//Default States
	void	toZero();
	void	toIdentity();



	//Default Accessors
	inline float   operator[](int index) const { return mat_array[index]; }
	inline float&  operator[](int index) { return mat_array[index]; }
	inline float   operator()(int row, int col) const { return mat_array[row + col * 3]; }
	inline float&  operator()(int row, int col) { return mat_array[row + col * 3]; }

	inline const NCLVector3&	getCol(int idx) const { return *((NCLVector3*)&mat_array[idx * 3]); }
	inline void				setCol(int idx, const NCLVector3& row) { memcpy(&mat_array[idx * 3], &row.x, 3 * sizeof(float)); }

	inline NCLVector3			getRow(int idx)	const { return NCLVector3(mat_array[idx], mat_array[3 + idx], mat_array[6 + idx]); }
	inline void				setRow(int idx, const NCLVector3& col) { mat_array[idx] = col.x; mat_array[3 + idx] = col.y; mat_array[6 + idx] = col.z; }



	//Common Matrix Properties
	inline NCLVector3			getScalingVector() const { return NCLVector3(_11, _22, _33); }
	inline void				setScalingVector(const NCLVector3& in) { _11 = in.x, _22 = in.y, _33 = in.z; }



	//Transformation Matrix
	static NCLMatrix3 rotation(float degrees, const NCLVector3 &axis);
	static NCLMatrix3 rotation(const NCLVector3 &forward_dir, const NCLVector3& up_dir = NCLVector3(0, 1, 0));
	static NCLMatrix3 scale(const NCLVector3 &scale);



	// Standard Matrix Functionality
	static NCLMatrix3 inverse(const NCLMatrix3& rhs);
	static NCLMatrix3 transpose(const NCLMatrix3& rhs);
	static NCLMatrix3 adjugate(const NCLMatrix3& m);

	static NCLMatrix3 outerProduct(const NCLVector3& a, const NCLVector3& b);



	// Additional Functionality
	float trace() const;
	float determinant() const;


	//Other representation of data.
	union
	{
		float	mat_array[9];
		struct {
			float _11, _12, _13;
			float _21, _22, _23;
			float _31, _32, _33;
		};
	};
};

NCLMatrix3& operator+=(NCLMatrix3& a, const NCLMatrix3& rhs);
NCLMatrix3& operator-=(NCLMatrix3& a, const NCLMatrix3& rhs);

NCLMatrix3 operator+(const NCLMatrix3& a, const NCLMatrix3& rhs);
NCLMatrix3 operator-(const NCLMatrix3& a, const NCLMatrix3& rhs);
NCLMatrix3 operator*(const NCLMatrix3& a, const NCLMatrix3& rhs);


NCLMatrix3& operator+=(NCLMatrix3& a, const float b);
NCLMatrix3& operator-=(NCLMatrix3& a, const float b);
NCLMatrix3& operator*=(NCLMatrix3& a, const float b);
NCLMatrix3& operator/=(NCLMatrix3& a, const float b);

NCLMatrix3 operator+(const NCLMatrix3& a, const float b);
NCLMatrix3 operator-(const NCLMatrix3& a, const float b);
NCLMatrix3 operator*(const NCLMatrix3& a, const float b);
NCLMatrix3 operator/(const NCLMatrix3& a, const float b);

NCLVector3 operator*(const NCLMatrix3& a, const NCLVector3& b);