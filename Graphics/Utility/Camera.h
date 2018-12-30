#pragma once

#include "../../Utilities/Maths/Matrix4.h"
#include "../../Utilities/Maths/Vector3.h"
#include "Frustum.h"

class SubMesh;

class SceneNode;

class Camera
{
public:
	Camera(void)
	{
		yaw = 0.0f;
		pitch = 0.0f;
	};

	Camera(float pitch, float yaw, NCLVector3 position)
	{
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
	}

	~Camera(void)
	{
	};

	void UpdateCamera(float msec = 10.0f);

	bool SubMeshIsInCameraView(SubMesh* submesh);

	bool SceneNodeIsInCameraView(SceneNode* sceneNode) const;

	//Builds a view matrix for the current camera variables, suitable for sending straight
	//to a vertex shader (i.e it's already an 'inverse camera matrix').
	NCLMatrix4 BuildViewMatrix();
	void UpdateViewFrustum(NCLMatrix4& projectionMatrix);

	//Gets position in world space
	NCLVector3 GetPosition() const
	{
		return position;
	}

	NCLVector3* GetPersistentPosition()
	{
		return &position;
	}

	//Sets position in world space
	void SetPosition(NCLVector3 val)
	{
		position = val;
	}

	//Gets yaw, in degrees
	float GetYaw() const
	{
		return yaw;
	}

	//Sets yaw, in degrees
	void SetYaw(float y)
	{
		yaw = y;
	}

	//Gets pitch, in degrees
	float GetPitch() const
	{
		return pitch;
	}

	//Sets pitch, in degrees
	void SetPitch(float p)
	{
		pitch = p;
	}

	NCLMatrix4 viewMatrix;

protected:
	Frustum viewFrustum;

	NCLVector3 position;
	float yaw;
	float pitch;
};
