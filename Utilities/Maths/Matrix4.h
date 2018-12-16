/******************************************************************************
Class:Matrix4
Implements:
Author:Rich Davison
Description:VERY simple 4 by 4 matrix class. Students are encouraged to modify 
this as necessary! Overloading the [] operator to allow acces to the values
array in a neater way might be a good start, as the floats that make the matrix 
up are currently public.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include "Vector3.h"
#include "Vector4.h"
#include "MathsCommon.h"

#include <matrix4x4.h>

class NCLVector3;

class NCLMatrix4	{
public:
	NCLMatrix4(void);
	NCLMatrix4(float elements[16]);
	NCLMatrix4(const aiMatrix4x4& matrix);
	~NCLMatrix4(void);

	float	values[16];

	//Set all matrix values to zero
	void	toZero();
	//Sets matrix to identity matrix (1.0 down the diagonal)
	void	toIdentity();

	void toASSIMPaiMatrix(aiMatrix4x4& m);

	//Gets the OpenGL position vector (floats 12,13, and 14)
	NCLVector3 getPositionVector() const;
	//Sets the OpenGL position vector (floats 12,13, and 14)
	void	setPositionVector(const NCLVector3 in);

	//Gets the scale vector (floats 1,5, and 10)
	NCLVector3 getScalingVector() const;
	//Sets the scale vector (floats 1,5, and 10)
	void	setScalingVector(const NCLVector3 &in);

	//Creates a rotation matrix that rotates by 'degrees' around the 'axis'
	//Analogous to glRotatef
	static NCLMatrix4 rotation(float degrees, const NCLVector3 &axis);

	//Creates a scaling matrix (puts the 'scale' vector down the diagonal)
	//Analogous to glScalef
	static NCLMatrix4 scale(const NCLVector3 &scale);

	//Creates a translation matrix (identity, with 'translation' vector at
	//floats 12, 13, and 14. Analogous to glTranslatef
	static NCLMatrix4 translation(const NCLVector3 &translation);

	//Creates a perspective matrix, with 'znear' and 'zfar' as the near and 
	//far planes, using 'aspect' and 'fov' as the aspect ratio and vertical
	//field of vision, respectively.
	static NCLMatrix4 perspective(float znear, float zfar, float aspect, float fov);

	//Creates an orthographic matrix with 'znear' and 'zfar' as the near and 
	//far planes, and so on. Descriptive variable names are a good thing!
	static NCLMatrix4 orthographic(float znear, float zfar,float right, float left, float top, float bottom);

	//Builds a view matrix suitable for sending straight to the vertex shader.
	//Puts the camera at 'from', with 'lookingAt' centered on the screen, with
	//'up' as the...up axis (pointing towards the top of the screen)
	static NCLMatrix4 buildViewMatrix(const NCLVector3 &from, const NCLVector3 &lookingAt, const NCLVector3 up = NCLVector3(0,1,0));

	NCLMatrix4 getTransposedRotation();

	//Multiplies 'this' matrix by matrix 'a'. Performs the multiplication in 'OpenGL' order (ie, backwards)
	inline NCLMatrix4 operator*(const NCLMatrix4 &a) const{	
		NCLMatrix4 out;
		//Students! You should be able to think up a really easy way of speeding this up...
		for(unsigned int r = 0; r < 4; ++r) {
			for(unsigned int c = 0; c < 4; ++c) {
				out.values[c + (r*4)] = 0.0f;
				for(unsigned int i = 0; i < 4; ++i) {
					out.values[c + (r*4)] += this->values[c+(i*4)] * a.values[(r*4)+i];
				}
			}
		}
		return out;
	}

	inline NCLVector3 operator*(const NCLVector3 &v) const {
		NCLVector3 vec;

		float temp;

		vec.x = v.x*values[0] + v.y*values[4] + v.z*values[8]  + values[12];
		vec.y = v.x*values[1] + v.y*values[5] + v.z*values[9]  + values[13];
		vec.z = v.x*values[2] + v.y*values[6] + v.z*values[10] + values[14];

		temp =  v.x*values[3] + v.y*values[7] + v.z*values[11] + values[15];

		vec.x = vec.x/temp;
		vec.y = vec.y/temp;
		vec.z = vec.z/temp;

		return vec;
	};

		inline NCLVector4 operator*(const NCLVector4 &v) const {
		return NCLVector4(
			v.x*values[0] + v.y*values[4] + v.z*values[8]  +v.w * values[12],
			v.x*values[1] + v.y*values[5] + v.z*values[9]  +v.w * values[13],
			v.x*values[2] + v.y*values[6] + v.z*values[10] +v.w * values[14],
			v.x*values[3] + v.y*values[7] + v.z*values[11] +v.w * values[15]
		);
	};


	inline const float& operator[](int i) const
	{
		return values[i];
	}

	inline float& operator[](int i)
	{
		return values[i];
	}

		//Added for GameTech - Code from taken from GLU library (all rights reserved).
		static NCLMatrix4 Inverse(const NCLMatrix4& rhs)
		{
			NCLMatrix4 inv;
			int i;

			inv[0] = rhs[5] * rhs[10] * rhs[15] -
				rhs[5] * rhs[11] * rhs[14] -
				rhs[9] * rhs[6] * rhs[15] +
				rhs[9] * rhs[7] * rhs[14] +
				rhs[13] * rhs[6] * rhs[11] -
				rhs[13] * rhs[7] * rhs[10];

			inv[4] = -rhs[4] * rhs[10] * rhs[15] +
				rhs[4] * rhs[11] * rhs[14] +
				rhs[8] * rhs[6] * rhs[15] -
				rhs[8] * rhs[7] * rhs[14] -
				rhs[12] * rhs[6] * rhs[11] +
				rhs[12] * rhs[7] * rhs[10];

			inv[8] = rhs[4] * rhs[9] * rhs[15] -
				rhs[4] * rhs[11] * rhs[13] -
				rhs[8] * rhs[5] * rhs[15] +
				rhs[8] * rhs[7] * rhs[13] +
				rhs[12] * rhs[5] * rhs[11] -
				rhs[12] * rhs[7] * rhs[9];

			inv[12] = -rhs[4] * rhs[9] * rhs[14] +
				rhs[4] * rhs[10] * rhs[13] +
				rhs[8] * rhs[5] * rhs[14] -
				rhs[8] * rhs[6] * rhs[13] -
				rhs[12] * rhs[5] * rhs[10] +
				rhs[12] * rhs[6] * rhs[9];

			inv[1] = -rhs[1] * rhs[10] * rhs[15] +
				rhs[1] * rhs[11] * rhs[14] +
				rhs[9] * rhs[2] * rhs[15] -
				rhs[9] * rhs[3] * rhs[14] -
				rhs[13] * rhs[2] * rhs[11] +
				rhs[13] * rhs[3] * rhs[10];

			inv[5] = rhs[0] * rhs[10] * rhs[15] -
				rhs[0] * rhs[11] * rhs[14] -
				rhs[8] * rhs[2] * rhs[15] +
				rhs[8] * rhs[3] * rhs[14] +
				rhs[12] * rhs[2] * rhs[11] -
				rhs[12] * rhs[3] * rhs[10];

			inv[9] = -rhs[0] * rhs[9] * rhs[15] +
				rhs[0] * rhs[11] * rhs[13] +
				rhs[8] * rhs[1] * rhs[15] -
				rhs[8] * rhs[3] * rhs[13] -
				rhs[12] * rhs[1] * rhs[11] +
				rhs[12] * rhs[3] * rhs[9];

			inv[13] = rhs[0] * rhs[9] * rhs[14] -
				rhs[0] * rhs[10] * rhs[13] -
				rhs[8] * rhs[1] * rhs[14] +
				rhs[8] * rhs[2] * rhs[13] +
				rhs[12] * rhs[1] * rhs[10] -
				rhs[12] * rhs[2] * rhs[9];

			inv[2] = rhs[1] * rhs[6] * rhs[15] -
				rhs[1] * rhs[7] * rhs[14] -
				rhs[5] * rhs[2] * rhs[15] +
				rhs[5] * rhs[3] * rhs[14] +
				rhs[13] * rhs[2] * rhs[7] -
				rhs[13] * rhs[3] * rhs[6];

			inv[6] = -rhs[0] * rhs[6] * rhs[15] +
				rhs[0] * rhs[7] * rhs[14] +
				rhs[4] * rhs[2] * rhs[15] -
				rhs[4] * rhs[3] * rhs[14] -
				rhs[12] * rhs[2] * rhs[7] +
				rhs[12] * rhs[3] * rhs[6];

			inv[10] = rhs[0] * rhs[5] * rhs[15] -
				rhs[0] * rhs[7] * rhs[13] -
				rhs[4] * rhs[1] * rhs[15] +
				rhs[4] * rhs[3] * rhs[13] +
				rhs[12] * rhs[1] * rhs[7] -
				rhs[12] * rhs[3] * rhs[5];

			inv[14] = -rhs[0] * rhs[5] * rhs[14] +
				rhs[0] * rhs[6] * rhs[13] +
				rhs[4] * rhs[1] * rhs[14] -
				rhs[4] * rhs[2] * rhs[13] -
				rhs[12] * rhs[1] * rhs[6] +
				rhs[12] * rhs[2] * rhs[5];

			inv[3] = -rhs[1] * rhs[6] * rhs[11] +
				rhs[1] * rhs[7] * rhs[10] +
				rhs[5] * rhs[2] * rhs[11] -
				rhs[5] * rhs[3] * rhs[10] -
				rhs[9] * rhs[2] * rhs[7] +
				rhs[9] * rhs[3] * rhs[6];

			inv[7] = rhs[0] * rhs[6] * rhs[11] -
				rhs[0] * rhs[7] * rhs[10] -
				rhs[4] * rhs[2] * rhs[11] +
				rhs[4] * rhs[3] * rhs[10] +
				rhs[8] * rhs[2] * rhs[7] -
				rhs[8] * rhs[3] * rhs[6];

			inv[11] = -rhs[0] * rhs[5] * rhs[11] +
				rhs[0] * rhs[7] * rhs[9] +
				rhs[4] * rhs[1] * rhs[11] -
				rhs[4] * rhs[3] * rhs[9] -
				rhs[8] * rhs[1] * rhs[7] +
				rhs[8] * rhs[3] * rhs[5];

			inv[15] = rhs[0] * rhs[5] * rhs[10] -
				rhs[0] * rhs[6] * rhs[9] -
				rhs[4] * rhs[1] * rhs[10] +
				rhs[4] * rhs[2] * rhs[9] +
				rhs[8] * rhs[1] * rhs[6] -
				rhs[8] * rhs[2] * rhs[5];

			float det = rhs[0] * inv[0] + rhs[1] * inv[4] + rhs[2] * inv[8] + rhs[3] * inv[12];

			if (det == 0)
			{
				inv.toIdentity();
				return inv;
			}

			det = 1.f / det;

			for (i = 0; i < 16; i++)
			{
				inv[i] = inv[i] * det;
			}

			return inv;
		}


	//Handy string output for the matrix. Can get a bit messy, but better than nothing!
	inline friend std::ostream& operator<<(std::ostream& o, const NCLMatrix4& m){
		o << "Mat4(";
		o << "\t"	<< m.values[0] << "," << m.values[1] << "," << m.values[2] << "," << m.values [3] << std::endl;
		o << "\t\t" << m.values[4] << "," << m.values[5] << "," << m.values[6] << "," << m.values [7] << std::endl;
		o << "\t\t" << m.values[8] << "," << m.values[9] << "," << m.values[10] << "," << m.values [11] << std::endl;
		o << "\t\t" << m.values[12] << "," << m.values[13] << "," << m.values[14] << "," << m.values [15] << " )" <<std::endl;
		return o;
	}
};

