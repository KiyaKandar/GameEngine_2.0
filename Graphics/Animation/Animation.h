#pragma once

#include <scene.h>
#include <vector>
#include <string.h>
#include <unordered_map>

class Mesh;
struct BoneInfo;
struct NodeAnimation;

struct MeshNode
{
	const aiNode* node;
	MeshNode* children;
	std::string nodeName;

	bool mapsToBone = false;
	bool hasAnimation = false;
};

class Animation
{
public:
	Animation(const std::string& animationName, const std::string& gameObjectId, Mesh* mesh, const aiAnimation* animation,
		const aiNode* rootNode, const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo);
	~Animation();

	bool hasGameObjectIdMatchOnly(const size_t& gameObjectId) const;
	bool hasAnimationIdMatchOnly(const size_t& animationId) const;
	bool hasIdMatch(const size_t& gameObjectId, const size_t& animationId) const;

	void incrementTimer(const double& deltaTime);
	void SetDurationToLerpFromPreviousAniamtion(const double& lerpDuration);
	bool meshIsOnScreen() const;

	void updateAnimationTransformState();
	void readAnimationState(std::vector<aiMatrix4x4>& animationState) const;

private:
	void validateElapsedTime();
	void validateLastKeyFrames(const double timeInTicks);

	void constructNodeList(const aiNode* rootNode);
	void addNode(MeshNode& parentNode, const aiNode* node, const int childIndex);

	void transformBones(std::vector<aiMatrix4x4>& transforms);
	void updateNode(const MeshNode& node, const aiMatrix4x4& parentTransform);
	void UpdateBoneTransformation(const MeshNode& meshNode, const aiMatrix4x4& childTransformation);
	void InterpolateNodeToFirstKeyFrameFromCurrentBoneTransform(const aiMatrix4x4& currentNodeTransform, const unsigned int boneIndex);

	const Mesh* mesh;
	double elapsedTime;
	double animationTime;
	double totalLerpDurationFromPreviousTransformation;
	double remainingLerpDurationFromPreviousTransformation;
	float interpolationFactor;

	std::vector<aiMatrix4x4> animationState;
	std::vector<BoneInfo>* boneInfo;
	std::unordered_map<std::string, NodeAnimation*> nodeAnimations;
	std::vector<NodeAnimation*> nodeAnimationRawStorage;
	MeshNode rootNode;

	const aiMatrix4x4 globalInverseTransform;
	const aiAnimation* animation;
	const size_t owningGameObjectId;
	const size_t animationId;
};

