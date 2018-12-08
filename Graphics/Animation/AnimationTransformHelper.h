#pragma once

#include <matrix4x4.h>
#include <vector3.h>
#include <quaternion.h>

struct NodeAnimation;

class AnimationTransformHelper
{
public:
	static void calculateNodeTransformation(aiMatrix4x4& transformation, NodeAnimation& nodeAnimation, const double& animationTime);

private:
	static void calculateInterpolatedKeyFrameTranslation(aiVector3D& translation, NodeAnimation& nodeAnimation, const double& animationTime);
	static void calculateInterpolatedKeyFrameRotation(aiQuaternion& rotation, NodeAnimation& nodeAnimation, const double& animationTime);
	static void calculateInterpolatedKeyFrameScale(aiVector3D& scale, NodeAnimation& nodeAnimation, const double& animationTime);

	static unsigned int findPositionKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime);
	static unsigned int findRotationKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime);
	static unsigned int findScalingKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime);

	static void interpolateVector3(aiVector3D& result, const aiVector3D& start, const aiVector3D& end, const float factor);
};