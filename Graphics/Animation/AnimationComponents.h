#pragma once

#include <matrix4x4.h>
#include <anim.h>
#include <vector3.h>
#include <quaternion.h>
#include <scene.h>
#include <functional>

typedef std::hash<std::string> Hash;

class Animation;

struct BlockedTransformComponents
{
	bool blockRotation = false;
	bool blockTranslation = false;
	bool blockScale = false;

	bool hasAnyComponentBlocked() const
	{
		return blockRotation || blockTranslation || blockScale;
	}
};

struct NodeTransformBlocker
{
	std::string nodeName;
	BlockedTransformComponents blockedComponents;
};

struct AnimationParams
{
	std::string animationName = "";
	double lerpToTime = 0.0;
	bool loop = false;
	NodeTransformBlocker transformBlocker;
};

struct BoneInfo
{
	aiMatrix4x4 boneOffset;
	aiMatrix4x4 finalTransformation;
	aiMatrix4x4 rawNodeTransform;
};

struct NodeAnimation
{
	NodeAnimation(const aiNodeAnim* animation)
	{
		this->animation = animation;
		ResetKeyFrameIndexes();
	}

	void ResetKeyFrameIndexes()
	{
		lastScalingKeyFrameIndex = 0;
		lastRotationKeyFrameIndex = 0;
		lastPositionKeyFrameIndex = 0;
	}

	const aiNodeAnim* animation;

	unsigned int lastScalingKeyFrameIndex;
	unsigned int lastRotationKeyFrameIndex;
	unsigned int lastPositionKeyFrameIndex;
};

struct DecomposedMatrix
{
	aiVector3D translation;
	aiQuaternion rotation;
	aiVector3D scale;
};

struct QueuedAnimation
{
	QueuedAnimation(std::string gameObjectId, const AnimationParams& params)
	{
		this->gameObjectId = gameObjectId;
		this->params = params;
	}

	QueuedAnimation() = default;

	std::string gameObjectId;
	AnimationParams params;
	AnimationParams transitionParams;
};

struct ActiveAnimation
{
	ActiveAnimation(Animation* animation, const QueuedAnimation& transition)
	{
		this->animation = animation;
		this->transition = transition;
	}

	bool hasTransition()
	{
		return transition.params.animationName != "";
	}

	Animation* animation;
	QueuedAnimation transition;
};