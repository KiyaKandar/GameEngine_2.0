#pragma once
/*
	AUTHOR: RICH DAVISON
*/

#include "Vector3.h"

class Plane
{
public:
	Plane(void)
	{
	};
	Plane(const NCLVector3& normal, float distance, bool normalise = false, NCLVector3 pos = NCLVector3());
	Plane(const NCLVector3& normal, NCLVector3 pos, bool normalise = false);

	~Plane(void)
	{
	};

	void SetNormal(const NCLVector3& normal)
	{
		this->normal = normal;
	}

	NCLVector3 GetNormal() const
	{
		return normal;
	}

	void SetDistance(float dist)
	{
		distance = dist;
	}

	float GetDistance() const
	{
		return distance;
	}

	bool SphereInPlane(const NCLVector3& position, float radius) const;
	bool SphereOutsidePlane(const NCLVector3& position, float radius) const;
	bool SphereIntersecting(const NCLVector3& position, float radius) const;
	bool PointInPlane(const NCLVector3& position) const;

	NCLVector3 position;
protected:
	NCLVector3 normal;
	float distance;
};
