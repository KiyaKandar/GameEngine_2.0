#pragma once

#include "../../Utilities/Maths/Vector4.h"
#include "../../Utilities/Maths/Vector3.h"
#include "../../Resource Management/Resources/Resource.h"

#include <algorithm>

struct LightData
{
	NCLVector4 lightPosition;
	NCLVector4 lightColour;
	float lightRadius;
	float isShadowCasting; //positive for true, negative for false
	float intensity;

	float padding;
};

struct SpotLightData
{
	NCLVector4 direction;
};

class Light : public Resource
{
public:
	Light(NCLVector3 position, NCLVector4 colour, float radius, float intensity, bool isShadowCasting, NCLVector4 direction = NCLVector4(0, 0, 0, 0))
		: Resource()
	{
		this->position = position;
		this->colour = colour;
		this->radius = radius;
		this->isShadowCasting = isShadowCasting;
		this->direction = direction;

		data.lightPosition = NCLVector4(position.x, position.y, position.z, 1.0f);
		data.lightColour = colour;
		data.lightRadius = radius;
		if (isShadowCasting)
			data.isShadowCasting = 1;
		else
			data.isShadowCasting = -1;
		data.intensity = intensity;
		spotLightData.direction = direction;
	}

	Light() : Resource() {
		this->position = NCLVector3(0, 0, 0);
		this->colour = NCLVector4(1, 1, 1, 1);
		this->radius = 0.0f;
		this->direction = NCLVector4(0, 0, 0, 0);
	}

	Light(const Light& rhs)
	{
		position = rhs.position;
		colour = rhs.colour;
		radius = rhs.radius;
		direction = rhs.direction;

		data.lightPosition = NCLVector4(position.x, position.y, position.z, 1.0f);
		data.lightColour = colour;
		data.lightRadius = radius;
		data.intensity = rhs.data.intensity;
		spotLightData.direction = rhs.direction;
	}

	~Light(void) {};

	static void AddLightsToArray(Light** lights, int numberExistingLights, int numberOfLights, Light lightToCopy);

	NCLVector3 GetPosition() const { return position; }
	void	SetPosition(NCLVector3 val)
	{
		position = val;
		data.lightPosition = NCLVector4(val.x, val.y, val.z, 1.0f);
	}

	float	GetRadius() const { return radius; }
	void	SetRadius(float val) { radius = val; }

	NCLVector4 GetColour() const { return colour; }
	void	SetColour(NCLVector4 val)
	{
		colour = val;
		data.lightColour = val;
	}

	void SetDirection(NCLVector4 val)
	{
		spotLightData.direction = val;
	}

	LightData GetData()
	{
		return data;
	}

	SpotLightData GetSpotData()
	{
		return spotLightData;
	}

	bool IsShadowCasting()
	{
		return isShadowCasting;
	}

protected:
	LightData data;
	SpotLightData spotLightData;
	NCLVector3 position;
	NCLVector4 colour;
	bool isShadowCasting;
	NCLVector4 direction;
	float radius;
};

