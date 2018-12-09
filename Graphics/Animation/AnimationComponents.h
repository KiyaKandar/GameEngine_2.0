#pragma once

#include <matrix4x4.h>
#include <anim.h>
#include <vector3.h>
#include <quaternion.h>
#include <scene.h>
#include <functional>

typedef std::hash<std::string> Hash;

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