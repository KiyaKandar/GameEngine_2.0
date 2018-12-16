#pragma once

#include <scene.h>
#include <vector>
#include <string.h>
#include <unordered_map>

class Mesh;
struct MeshNode;
struct BoneInfo;
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

	void blockTransformationForNode(const std::string& nodeName, const BlockedTransformComponents& blockedComponents);

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
	void ResetAllKeyFrameIndexes();

	void constructNodeList(const aiNode* rootNode);
	void addNode(MeshNode& parentNode, const aiNode* node, const int childIndex);

	void getChildNodeByName(MeshNode*& foundNode, MeshNode& childNode, const std::string& nodeName);
	void searchChildNodeToBlockNodeTransformation(const std::string& nodeName, const BlockedTransformComponents& blockedComponents);
	void unblockChildNodeTransformation(MeshNode& childNode);

	void transformBones(std::vector<aiMatrix4x4>& transforms);
	void updateNode(MeshNode& node, const aiMatrix4x4& parentTransform);
	void updateBoneTransformation(const MeshNode& meshNode, const aiMatrix4x4& childTransformation);
	void interpolateNodeToFirstKeyFrameFromCurrentBoneTransform(const aiMatrix4x4& currentNodeTransform, const unsigned int boneIndex);

	void recursivelyDrawBones(const MeshNode& parentNode, const aiMatrix4x4& parentTransform);

	void RemoveSceneNodeTransformFromBones(const MeshNode& node, const aiMatrix4x4& sceneNodeTransform, 
		const BlockedTransformComponents& blockedComponents);
	void updateRawTransform(const MeshNode& node, aiMatrix4x4 transform);

	const Mesh* mesh;
	double elapsedTime;
	double animationTime;
	bool looping;
	double totalLerpDurationFromPreviousTransformation;
	double remainingLerpDurationFromPreviousTransformation;
	float interpolationFactor;

	std::vector<aiMatrix4x4> animationState;
	std::vector<BoneInfo>* boneInfo;
	std::unordered_map<std::string, NodeAnimation*> nodeAnimations;
	std::vector<NodeAnimation*> nodeAnimationRawStorage;
	MeshNode* rootNode;

	const aiMatrix4x4 globalInverseTransform;
	const aiAnimation* animation;
	const size_t owningGameObjectId;
	const size_t animationId;
	const std::string owningGameObjectName;
};

