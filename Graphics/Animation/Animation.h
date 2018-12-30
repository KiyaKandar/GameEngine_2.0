#pragma once

#include "Skeleton.h"

#include <scene.h>
#include <vector>
#include <string.h>

class Mesh;
class SkeletonDisplay;
struct MeshNode;
struct NodeAnimation;
struct BlockedTransformComponents;
struct NodeTransformSpecifier;

class Animation
{
public:
	Animation(const std::string& animationName, const std::string& gameObjectId, Mesh* mesh, const aiAnimation* animation,
		const aiNode* rootNode, const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo);
	~Animation();

	const std::string GetOwningGameObjectName() const;
	bool HasGameObjectIdMatchOnly(const size_t& gameObjectId) const;
	bool HasAnimationIdMatchOnly(const size_t& animationId) const;
	bool HasIdMatch(const size_t& gameObjectId, const size_t& animationId) const;
	bool IsLooping() const;

	void UpdateSceneNodeTransformFromNode(const NodeTransformSpecifier& nodeSpecifier);
	aiMatrix4x4 GetCurrentTransformOfSceneNodeTransformerNode(const std::string nodeName);
	void DebugDrawSkeleton(const aiMatrix4x4& parentTransform);

	void SetBlockedSkeletonNodeTransforms(const std::string& nodeName, const BlockedTransformComponents& blockedComponents);

	void IncrementTimer(const double& deltaTime);
	void Reset();

	void SetDurationToLerpFromPreviousAniamtion(const double& lerpDuration);
	void SetLooping(const bool looping);

	bool FinishedPlaying() const;
	bool MeshIsOnScreen() const;

	void UpdateAnimationTransformState();
	void ReadAnimationState(std::vector<aiMatrix4x4>& animationState) const;

private:
	void ValidateLastKeyFrames(const double timeInTicks);
	void TransformBones(std::vector<aiMatrix4x4>& transforms);
	void RemoveSceneNodeTransformFromBones(const MeshNode& node, const aiMatrix4x4& sceneNodeTransform, 
		const BlockedTransformComponents& blockedComponents);

	const Mesh* mesh;
	double elapsedTime;
	double animationTime;
	bool looping;
	double totalLerpDurationFromPreviousTransformation;
	double remainingLerpDurationFromPreviousTransformation;
	float interpolationFactor;

	std::vector<aiMatrix4x4> animationState;

	Skeleton* skeleton;
	SkeletonDisplay* debugSkeletonDisplay;

	const aiAnimation* animation;
	const size_t owningGameObjectId;
	const size_t animationId;
	const std::string owningGameObjectName;
};

