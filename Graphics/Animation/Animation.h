#pragma once

#include <matrix4x4.h>
#include <anim.h>
#include <vector3.h>
#include <quaternion.h>
#include <scene.h>
#include <vector>
#include <functional>
#include <string.h>
#include <unordered_map>

class Mesh;

struct BoneInfo
{
	aiMatrix4x4 boneOffset;
	aiMatrix4x4 finalTransformation;
};

struct MeshNode
{
	const aiNode* node;
	MeshNode* children;
	std::string nodeName;

	bool mapsToBone = false;
	bool hasAnimation = false;
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

class Animation
{
public:
	Animation(Mesh* mesh, const aiAnimation* animation, const aiNode* rootNode, const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo);
	~Animation();

	bool hasIdMatch(const size_t& id) const;
	void incrementTimer(const double& deltaTime);
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

	const Mesh* mesh;
	double elapsedTime;
	double animationTime;

	std::vector<aiMatrix4x4> animationState;
	std::vector<BoneInfo>* boneInfo;
	std::unordered_map<std::string, NodeAnimation*> nodeAnimations;
	std::vector<NodeAnimation*> nodeAnimationRawStorage;
	MeshNode rootNode;

	const aiMatrix4x4 globalInverseTransform;
	const aiAnimation* animation;
	const size_t animationId;
};

