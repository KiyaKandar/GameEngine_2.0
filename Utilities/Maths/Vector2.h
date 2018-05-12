#pragma once
/*
Class:Vector2
Implements:
Author:Rich Davison
Description:VERY simple Vector2 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#include <iostream>

class NCLVector2	{
public:
	NCLVector2(void) {
		toZero();
	}

	NCLVector2(const float x, const float y) {
		this->x = x;
		this->y = y;
	}

	~NCLVector2(void){}

	float x;
	float y;

	void toZero(){
		x = 0.0f;
		y = 0.0f;
	}

	inline friend std::ostream& operator<<(std::ostream& o, const NCLVector2& v){
		o << "Vector2(" << v.x << "," << v.y << ")" << std::endl;
		return o;
	}

	inline NCLVector2  operator-(const NCLVector2  &a) const{
		return NCLVector2(x - a.x,y - a.y);
	}

	inline NCLVector2  operator+(const NCLVector2  &a) const{
		return NCLVector2(x + a.x,y + a.y);
	}

	inline NCLVector2  operator*(const NCLVector2  &a) const {
		return NCLVector2(x * a.x, y * a.y);
	}

	inline NCLVector2  operator/(const NCLVector2  &a) const {
		return NCLVector2(x / a.x, y / a.y);
	};
};

