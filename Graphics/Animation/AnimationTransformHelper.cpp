#include "AnimationTransformHelper.h"

#include "AnimationComponents.h"

#include <anim.h>

BlockedTransformComponents AnimationTransformHelper::defaultBlockedComponents = BlockedTransformComponents();

void AnimationTransformHelper::CalculateNodeTransformation(aiMatrix4x4& transformation, NodeAnimation& nodeAnimation, 
	const double& animationTime, const BlockedTransformComponents& blockedComponents)
{
	DecomposedMatrix storedTransformation;
	if (blockedComponents.HasAnyComponentBlocked())
	{
		transformation.Decompose(storedTransformation.scale, storedTransformation.rotation, storedTransformation.translation);
	}

	aiMatrix4x4 translationTransform, rotationTransform, scalingTransform;

	CalculateKeyFrameTranslation(translationTransform, nodeAnimation, animationTime, 
		blockedComponents.blockTranslation, storedTransformation.translation);
	CalculateKeyFrameRotation(rotationTransform, nodeAnimation, animationTime,
		blockedComponents.blockRotation, storedTransformation.rotation);
	CalculateKeyFrameScale(scalingTransform, nodeAnimation, animationTime, 
		blockedComponents.blockScale, storedTransformation.scale);

	transformation = translationTransform * rotationTransform * scalingTransform;
}

void AnimationTransformHelper::RemoveBlockedComponentsFromTransform(aiMatrix4x4& result, const aiMatrix4x4& originalTransformation, 
	const BlockedTransformComponents& blockedComponents)
{
	DecomposedMatrix transformationComponents;
	originalTransformation.Decompose(transformationComponents.scale, transformationComponents.rotation, transformationComponents.translation);

	ComposeMatrix(result, transformationComponents, blockedComponents);
}

void AnimationTransformHelper::InterpolateVector3(aiVector3D& result, const aiVector3D& start, const aiVector3D& end, const float factor)
{
	aiVector3D delta = end - start;
	result = start + (float)factor * delta;
}

void AnimationTransformHelper::InterpolateDecomposedMatrices(DecomposedMatrix& result, const DecomposedMatrix& start, 
	const DecomposedMatrix& end, const float factor)
{
	AnimationTransformHelper::InterpolateVector3(result.translation, start.translation, end.translation, factor);
	AnimationTransformHelper::InterpolateVector3(result.scale, start.scale, end.scale, factor);
	aiQuaternion::Interpolate(result.rotation, start.rotation, end.rotation, factor);
	result.rotation = result.rotation.Normalize();
}

void AnimationTransformHelper::ComposeMatrix(aiMatrix4x4& result, const DecomposedMatrix& decomposedMatrix,
	const BlockedTransformComponents& blockedComponents)
{
	aiMatrix4x4 translationTransform;
	aiMatrix4x4 rotationTransform;
	aiMatrix4x4 scalingTransform;

	if (!blockedComponents.blockTranslation)
	{
		aiMatrix4x4::Translation(decomposedMatrix.translation, translationTransform);
	}

	if (!blockedComponents.blockRotation)
	{
		rotationTransform = aiMatrix4x4(decomposedMatrix.rotation.GetMatrix());
	}

	if (!blockedComponents.blockScale)
	{
		aiMatrix4x4::Scaling(decomposedMatrix.scale, scalingTransform);
	}

	result = translationTransform * rotationTransform * scalingTransform;
}

void AnimationTransformHelper::CalculateKeyFrameTranslation(aiMatrix4x4& translationTransform, NodeAnimation& nodeAnimation,
	const double& animationTime, const bool blocked, const aiVector3D& defaultTranslation)
{
	if (blocked)
	{
		aiMatrix4x4::Translation(defaultTranslation, translationTransform);
	}
	else
	{
		aiVector3D translation;
		AnimationTransformHelper::CalculateInterpolatedKeyFrameTranslation(translation, nodeAnimation, animationTime);
		aiMatrix4x4::Translation(translation, translationTransform);
	}
}

void AnimationTransformHelper::CalculateKeyFrameRotation(aiMatrix4x4& rotationTransform, NodeAnimation& nodeAnimation, 
	const double& animationTime, const bool blocked, const aiQuaternion& defaultRotation)
{
	if (blocked)
	{
		rotationTransform = aiMatrix4x4(defaultRotation.GetMatrix());
	}
	else
	{
		aiQuaternion rotation;
		AnimationTransformHelper::CalculateInterpolatedKeyFrameRotation(rotation, nodeAnimation, animationTime);
		rotationTransform = aiMatrix4x4(rotation.GetMatrix());
	}
}

void AnimationTransformHelper::CalculateKeyFrameScale(aiMatrix4x4& scalingTransform, NodeAnimation& nodeAnimation,
	const double& animationTime, const bool blocked, const aiVector3D& defaultScale)
{
	if (blocked)
	{
		aiMatrix4x4::Scaling(defaultScale, scalingTransform);
	}
	else
	{
		aiVector3D scale;
		AnimationTransformHelper::CalculateInterpolatedKeyFrameScale(scale, nodeAnimation, animationTime);
		aiMatrix4x4::Scaling(scale, scalingTransform);
	}
}

void AnimationTransformHelper::CalculateInterpolatedKeyFrameTranslation(aiVector3D& translation, NodeAnimation& nodeAnimation, const double& animationTime)
{
	const aiNodeAnim* animation = nodeAnimation.animation;
	if (animation->mNumPositionKeys == 1) 
	{
		translation = animation->mPositionKeys[0].mValue;
	}
	else
	{
		unsigned int positionIndex = FindPositionKeyFrameIndex(nodeAnimation, animationTime);
		unsigned int nextPositionIndex = (positionIndex + 1);

		double keyFrameTimeDifference = (animation->mPositionKeys[nextPositionIndex].mTime - animation->mPositionKeys[positionIndex].mTime);
		double interpolationFactor = (animationTime - animation->mPositionKeys[positionIndex].mTime) / keyFrameTimeDifference;
		InterpolateVector3(translation, animation->mPositionKeys[positionIndex].mValue, animation->mPositionKeys[nextPositionIndex].mValue, interpolationFactor);
	}
}

void AnimationTransformHelper::CalculateInterpolatedKeyFrameRotation(aiQuaternion& rotation, NodeAnimation& nodeAnimation, const double& animationTime)
{
	const aiNodeAnim* animation = nodeAnimation.animation;
	if (animation->mNumRotationKeys == 1)
	{
		rotation = animation->mRotationKeys[0].mValue;
	}
	else
	{
		unsigned int rotationIndex = FindRotationKeyFrameIndex(nodeAnimation, animationTime);
		unsigned int nextRotationIndex = rotationIndex + 1;

		double keyFrameTimeDifference = (animation->mRotationKeys[nextRotationIndex].mTime - animation->mRotationKeys[rotationIndex].mTime);
		double interpolationFactor = (animationTime - animation->mRotationKeys[rotationIndex].mTime) / keyFrameTimeDifference;

		aiQuaternion::Interpolate(rotation, animation->mRotationKeys[rotationIndex].mValue, 
			animation->mRotationKeys[nextRotationIndex].mValue, (float)interpolationFactor);
		rotation = rotation.Normalize();
	}
}

void AnimationTransformHelper::CalculateInterpolatedKeyFrameScale(aiVector3D& scale, NodeAnimation& nodeAnimation, const double& animationTime)
{
	const aiNodeAnim* animation = nodeAnimation.animation;
	if (animation->mNumScalingKeys == 1) 
	{
		scale = animation->mScalingKeys[0].mValue;
	}
	else
	{
		unsigned int scalingIndex = FindScalingKeyFrameIndex(nodeAnimation, animationTime);
		unsigned int nextScalingIndex = scalingIndex + 1;

		double keyFrameTimeDifference = (animation->mScalingKeys[nextScalingIndex].mTime - animation->mScalingKeys[scalingIndex].mTime);
		double interpolationFactor = (animationTime - animation->mScalingKeys[scalingIndex].mTime) / keyFrameTimeDifference;
		InterpolateVector3(scale, animation->mScalingKeys[scalingIndex].mValue, animation->mScalingKeys[nextScalingIndex].mValue, interpolationFactor);
	}
}

unsigned int AnimationTransformHelper::FindPositionKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime)
{
	for (unsigned int i = nodeAnimation.lastPositionKeyFrameIndex; i < nodeAnimation.animation->mNumPositionKeys - 1; i++)
	{
		if (animationTime < nodeAnimation.animation->mPositionKeys[i + 1].mTime)
		{
			nodeAnimation.lastPositionKeyFrameIndex = i;
			return i;
		}
	}

	return 0;
}

unsigned int AnimationTransformHelper::FindRotationKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime)
{
	for (unsigned int i = nodeAnimation.lastRotationKeyFrameIndex; i < nodeAnimation.animation->mNumRotationKeys - 1; i++)
	{
		if (animationTime < nodeAnimation.animation->mRotationKeys[i + 1].mTime)
		{
			nodeAnimation.lastRotationKeyFrameIndex = i;
			return i;
		}
	}

	return 0;
}

unsigned int AnimationTransformHelper::FindScalingKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime)
{
	for (unsigned int i = nodeAnimation.lastScalingKeyFrameIndex; i < nodeAnimation.animation->mNumScalingKeys - 1; i++)
	{
		if (animationTime < nodeAnimation.animation->mScalingKeys[i + 1].mTime)
		{
			nodeAnimation.lastScalingKeyFrameIndex = i;
			return i;
		}
	}

	return 0;
}
