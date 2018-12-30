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
	static void CalculateNodeTransformation(aiMatrix4x4& transformation, NodeAnimation& nodeAnimation, 
		const double& animationTime, const BlockedTransformComponents& blockedComponents);
	static void RemoveBlockedComponentsFromTransform(aiMatrix4x4& result, const aiMatrix4x4& originalTransformation,
		const BlockedTransformComponents& blockedComponents);

	static void InterpolateVector3(aiVector3D& result, const aiVector3D& start, const aiVector3D& end, const float factor);
	static void InterpolateDecomposedMatrices(DecomposedMatrix& result, const DecomposedMatrix& start, 
		const DecomposedMatrix& end, const float factor);
	static void ComposeMatrix(aiMatrix4x4& result, const DecomposedMatrix& decomposedMatrix, 
		const BlockedTransformComponents& blockedComponents = defaultBlockedComponents);

private:
	static void CalculateKeyFrameTranslation(aiMatrix4x4& translationTransform, NodeAnimation& nodeAnimation,
		const double& animationTime, const bool blocked, const aiVector3D& defaultTranslation);
	static void CalculateKeyFrameRotation(aiMatrix4x4& rotationTransform, NodeAnimation& nodeAnimation,
		const double& animationTime, const bool blocked, const aiQuaternion& defaultRotation);
	static void CalculateKeyFrameScale(aiMatrix4x4& scalingTransform, NodeAnimation& nodeAnimation,
		const double& animationTime, const bool blocked, const aiVector3D& defaultScale);

	static void CalculateInterpolatedKeyFrameTranslation(aiVector3D& translation, NodeAnimation& nodeAnimation, const double& animationTime);
	static void CalculateInterpolatedKeyFrameRotation(aiQuaternion& rotation, NodeAnimation& nodeAnimation, const double& animationTime);
	static void CalculateInterpolatedKeyFrameScale(aiVector3D& scale, NodeAnimation& nodeAnimation, const double& animationTime);

	static unsigned int FindPositionKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime);
	static unsigned int FindRotationKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime);
	static unsigned int FindScalingKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime);

	static BlockedTransformComponents defaultBlockedComponents;
};