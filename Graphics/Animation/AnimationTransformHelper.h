#pragma once

#include <matrix4x4.h>
#include <vector3.h>
#include <quaternion.h>

struct NodeAnimation;
struct DecomposedMatrix;
struct BlockedTransformComponents;

class AnimationTransformHelper
{
public:
	static void calculateNodeTransformation(aiMatrix4x4& transformation, NodeAnimation& nodeAnimation, 
		const double& animationTime, const BlockedTransformComponents& blockedComponents);
	static void removeBlockedComponentsFromTransform(aiMatrix4x4& result, const aiMatrix4x4& originalTransformation,
		const BlockedTransformComponents& blockedComponents);

	static void interpolateVector3(aiVector3D& result, const aiVector3D& start, const aiVector3D& end, const float factor);
	static void interpolateDecomposedMatrices(DecomposedMatrix& result, const DecomposedMatrix& start, 
		const DecomposedMatrix& end, const float factor);
	static void composeMatrix(aiMatrix4x4& result, const DecomposedMatrix& decomposedMatrix, 
		const BlockedTransformComponents& blockedComponents = defaultBlockedComponents);

private:
	static void calculateKeyFrameTranslation(aiMatrix4x4& translationTransform, NodeAnimation& nodeAnimation,
		const double& animationTime, const bool blocked, const aiVector3D& defaultTranslation);
	static void calculateKeyFrameRotation(aiMatrix4x4& rotationTransform, NodeAnimation& nodeAnimation,
		const double& animationTime, const bool blocked, const aiQuaternion& defaultRotation);
	static void calculateKeyFrameScale(aiMatrix4x4& scalingTransform, NodeAnimation& nodeAnimation,
		const double& animationTime, const bool blocked, const aiVector3D& defaultScale);

	static void calculateInterpolatedKeyFrameTranslation(aiVector3D& translation, NodeAnimation& nodeAnimation, const double& animationTime);
	static void calculateInterpolatedKeyFrameRotation(aiQuaternion& rotation, NodeAnimation& nodeAnimation, const double& animationTime);
	static void calculateInterpolatedKeyFrameScale(aiVector3D& scale, NodeAnimation& nodeAnimation, const double& animationTime);

	static unsigned int findPositionKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime);
	static unsigned int findRotationKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime);
	static unsigned int findScalingKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime);

	static BlockedTransformComponents defaultBlockedComponents;
};