#include "AnimationTransformHelper.h"

#include "Animation.h"

#include <anim.h>

void AnimationTransformHelper::calculateNodeTransformation(aiMatrix4x4& transformation, NodeAnimation& nodeAnimation, const double& animationTime)
{
	aiVector3D translation;
	AnimationTransformHelper::calculateInterpolatedKeyFrameTranslation(translation, nodeAnimation, animationTime);
	aiMatrix4x4 translationTransform;
	aiMatrix4x4::Translation(translation, translationTransform);

	aiQuaternion rotation;
	AnimationTransformHelper::calculateInterpolatedKeyFrameRotation(rotation, nodeAnimation, animationTime);
	aiMatrix4x4 rotationTransform = aiMatrix4x4(rotation.GetMatrix());

	aiVector3D scale;
	AnimationTransformHelper::calculateInterpolatedKeyFrameScale(scale, nodeAnimation, animationTime);
	aiMatrix4x4 scalingTransform;
	aiMatrix4x4::Scaling(scale, scalingTransform);

	transformation = translationTransform * rotationTransform * scalingTransform;
}

void AnimationTransformHelper::calculateInterpolatedKeyFrameTranslation(aiVector3D& translation, NodeAnimation& nodeAnimation, const double& animationTime)
{
	const aiNodeAnim* animation = nodeAnimation.animation;
	if (animation->mNumPositionKeys == 1) 
	{
		translation = animation->mPositionKeys[0].mValue;
	}
	else
	{
		unsigned int positionIndex = findPositionKeyFrameIndex(nodeAnimation, animationTime);
		unsigned int nextPositionIndex = (positionIndex + 1);

		double keyFrameTimeDifference = (animation->mPositionKeys[nextPositionIndex].mTime - animation->mPositionKeys[positionIndex].mTime);
		double interpolationFactor = (animationTime - animation->mPositionKeys[positionIndex].mTime) / keyFrameTimeDifference;
		interpolateVector3(translation, animation->mPositionKeys[positionIndex].mValue, animation->mPositionKeys[nextPositionIndex].mValue, interpolationFactor);
	}
}

void AnimationTransformHelper::calculateInterpolatedKeyFrameRotation(aiQuaternion& rotation, NodeAnimation& nodeAnimation, const double& animationTime)
{
	const aiNodeAnim* animation = nodeAnimation.animation;
	if (animation->mNumRotationKeys == 1)
	{
		rotation = animation->mRotationKeys[0].mValue;
	}
	else
	{
		unsigned int rotationIndex = findRotationKeyFrameIndex(nodeAnimation, animationTime);
		unsigned int nextRotationIndex = rotationIndex + 1;

		double keyFrameTimeDifference = (animation->mRotationKeys[nextRotationIndex].mTime - animation->mRotationKeys[rotationIndex].mTime);
		double interpolationFactor = (animationTime - animation->mRotationKeys[rotationIndex].mTime) / keyFrameTimeDifference;

		aiQuaternion::Interpolate(rotation, animation->mRotationKeys[rotationIndex].mValue, 
			animation->mRotationKeys[nextRotationIndex].mValue, (float)interpolationFactor);
		rotation = rotation.Normalize();
	}
}

void AnimationTransformHelper::calculateInterpolatedKeyFrameScale(aiVector3D& scale, NodeAnimation& nodeAnimation, const double& animationTime)
{
	const aiNodeAnim* animation = nodeAnimation.animation;
	if (animation->mNumScalingKeys == 1) 
	{
		scale = animation->mScalingKeys[0].mValue;
	}
	else
	{
		unsigned int scalingIndex = findScalingKeyFrameIndex(nodeAnimation, animationTime);
		unsigned int nextScalingIndex = scalingIndex + 1;

		double keyFrameTimeDifference = (animation->mScalingKeys[nextScalingIndex].mTime - animation->mScalingKeys[scalingIndex].mTime);
		double interpolationFactor = (animationTime - animation->mScalingKeys[scalingIndex].mTime) / keyFrameTimeDifference;
		interpolateVector3(scale, animation->mScalingKeys[scalingIndex].mValue, animation->mScalingKeys[nextScalingIndex].mValue, interpolationFactor);
	}
}

unsigned int AnimationTransformHelper::findPositionKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime)
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

unsigned int AnimationTransformHelper::findRotationKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime)
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

unsigned int AnimationTransformHelper::findScalingKeyFrameIndex(NodeAnimation& nodeAnimation, const double& animationTime)
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

void AnimationTransformHelper::interpolateVector3(aiVector3D& result, const aiVector3D& start, const aiVector3D& end, const float factor)
{
	aiVector3D delta = end - start;
	result = start + (float)factor * delta;
}
