#pragma once
/*
	AUTHOR: RICH DAVISON
*/


#include "Vector3.h"

class Plane
{
public:
	Plane(void) {};
	Plane(const NCLVector3 &normal, float distance, bool normalise = false, NCLVector3 pos = NCLVector3());
	Plane(const NCLVector3 &normal, NCLVector3 pos, bool normalise = false);
	~Plane(void) {};

	void	setNormal(const NCLVector3 &normal)	{ this->normal = normal; }
	NCLVector3 getNormal()					const	{ return normal; }

	void	setDistance(float dist)				{ distance = dist; }
	float	getDistance()				const	{ return distance; }

	bool sphereInPlane(const NCLVector3 &position, float radius)		const;
	bool sphereOutsidePlane(const NCLVector3& position, float radius)	const;
	bool sphereIntersecting(const NCLVector3& position, float radius)	const;
	bool pointInPlane(const NCLVector3& position) const;

	NCLVector3 position;
protected:
	NCLVector3 normal;
	float distance;

};

