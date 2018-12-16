#pragma once

#include "Skeleton.h"

#include <scene.h>
#include <vector>
#include <string.h>

class Mesh;
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

	const std::string getOwningGameObjectName() const;
	bool hasGameObjectIdMatchOnly(const size_t& gameObjectId) const;
	bool hasAnimationIdMatchOnly(const size_t& animationId) const;
	bool hasIdMatch(const size_t& gameObjectId, const size_t& animationId) const;
	bool isLooping() const;

	void updateSceneNodeTransformFromNode(const NodeTransformSpecifier& nodeSpecifier);
	aiMatrix4x4 getCurrentTransformOfSceneNodeTransformerNode(const std::string nodeName);
	void debugDrawSkeleton(const aiMatrix4x4& parentTransform);

	void setBlockedSkeletonNodeTransforms(const std::string& nodeName, const BlockedTransformComponents& blockedComponents);

	void incrementTimer(const double& deltaTime);
	void reset();

	void setDurationToLerpFromPreviousAniamtion(const double& lerpDuration);
	void setLooping(const bool looping);

	bool finishedPlaying() const;
	bool meshIsOnScreen() const;

	void updateAnimationTransformState();
	void readAnimationState(std::vector<aiMatrix4x4>& animationState) const;

private:
	void validateLastKeyFrames(const double timeInTicks);
	void transformBones(std::vector<aiMatrix4x4>& transforms);
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

	const aiAnimation* animation;
	const size_t owningGameObjectId;
	const size_t animationId;
	const std::string owningGameObjectName;
};

