#pragma once

#include "../../Utilities/Maths/Matrix4.h"
#include "../../Utilities/Maths/Vector3.h"
#include "Frustum.h"

class SubMesh;

class SceneNode;

class Camera
{
public:
	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
	};

	Camera(float pitch, float yaw, NCLVector3 position) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
	}

	~Camera(void) {
	};

	void updateCamera(float msec = 10.0f);

	bool subMeshIsInCameraView(SubMesh* submesh);

	bool sceneNodeIsInCameraView(SceneNode* sceneNode);

	//Builds a view matrix for the current camera variables, suitable for sending straight
	//to a vertex shader (i.e it's already an 'inverse camera matrix').
	NCLMatrix4 buildViewMatrix();
	void updateViewFrustum(NCLMatrix4& projectionMatrix);

	//Gets position in world space
	NCLVector3 getPosition() const {
		return position;
	}

	NCLVector3* getPersistentPosition()
	{
		return &position;
	}

	//Sets position in world space
	void setPosition(NCLVector3 val)
	{
		position = val;

	}

	//Gets yaw, in degrees
	float	getYaw()   const { return yaw; }
	//Sets yaw, in degrees
	void	setYaw(float y) { yaw = y; }

	//Gets pitch, in degrees
	float	getPitch() const { return pitch; }
	//Sets pitch, in degrees
	void	setPitch(float p) { pitch = p; }

	NCLMatrix4 viewMatrix;

protected:
	Frustum viewFrustum;

	NCLVector3 position;
	float	yaw;
	float	pitch;
};

