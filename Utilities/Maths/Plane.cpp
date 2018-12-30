#include "Plane.h"

Plane::Plane(const NCLVector3& normal, float distance, bool normalise, NCLVector3 pos)
{
	if (normalise)
	{
		float length = sqrt(NCLVector3::dot(normal, normal));

		this->normal = normal / length;
		this->distance = distance / length;
	}
	else
	{
		this->normal = normal;
		this->distance = distance;
	}

	position = pos;
}

Plane::Plane(const NCLVector3& normal, NCLVector3 pos, bool normalise)
{
	if (normalise)
	{
		float length = sqrt(NCLVector3::dot(normal, normal));

		this->normal = normal / length;
		this->distance = pos.Length() / length;
	}
	else
	{
		this->normal = normal;
		this->distance = pos.Length();
	}

	position = pos;
}

bool Plane::SphereInPlane(const NCLVector3& position, float radius) const
{
	if (NCLVector3::dot(position, normal) + distance <= -radius)
	{
		return false;
	}
	return true;

	//return -(Vector3::Dot(normal, position) + distance) > radius;
}

bool Plane::SphereOutsidePlane(const NCLVector3& position, float radius) const
{
	if (-(NCLVector3::dot(position, normal) + distance) <= -radius)
	{
		return false;
	}
	return true;

	//return (Vector3::Dot(normal, position) + distance) > radius;
}

bool Plane::SphereIntersecting(const NCLVector3& position, float radius) const
{
	return (std::fabs(NCLVector3::dot(position, normal) + distance)) <= radius;
}

bool Plane::PointInPlane(const NCLVector3& position) const
{
	if (NCLVector3::dot(position, normal) + distance < 0.0f)
	{
		return false;
	}

	return true;
}
