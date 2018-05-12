#include "Camera.h"

#include "../Scene Management/SceneNode.h"

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::updateCamera(float msec) {
	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = min(pitch, 90.0f);
	pitch = max(pitch, -90.0f);

	if (yaw <0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

}

bool Camera::subMeshIsInCameraView(SubMesh* submesh)
{
	const NCLVector3 position = submesh->GetTransform().getPositionVector();
	const float radius = submesh->GetBoundingRadius();

	return viewFrustum.insideFrustum(position, radius);
}

bool Camera::sceneNodeIsInCameraView(SceneNode * sceneNode)
{
	const NCLVector3 position = sceneNode->GetWorldTransform().getPositionVector();
	const float radius = sceneNode->getRadius();


	//return viewFrustum.insideFrustum(position, radius);
	return true;
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
NCLMatrix4 Camera::buildViewMatrix() {
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	NCLMatrix4 matrix = NCLMatrix4::rotation(-pitch, NCLVector3(1, 0, 0)) *
		NCLMatrix4::rotation(-yaw, NCLVector3(0, 1, 0)) *
		NCLMatrix4::translation(-position);

	viewMatrix = matrix;

	return matrix;
}

void Camera::updateViewFrustum(NCLMatrix4& projectionMatrix)
{
	viewFrustum.fromMatrix(projectionMatrix * viewMatrix);
};
