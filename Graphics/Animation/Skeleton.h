#pragma once

#include "AnimationComponents.h"

#include <map>
#include <scene.h>
#include <string.h>
#include <unordered_map>

struct BoneInfo;
class Mesh;

struct MeshNode
{
	const aiNode* node;
	MeshNode* children;
	MeshNode* parent;
	std::string nodeName;
	aiMatrix4x4 rawTransform;

	bool mapsToBone = false;
	bool hasAnimation = false;
	BlockedTransformComponents blockedComponents;
};

class Skeleton
{
public:
	Skeleton(const aiAnimation* animation, const aiNode* aiRootNode, const Mesh* mesh, 
		std::vector<BoneInfo>* boneInfo, const std::map<std::string, unsigned int>* sharedBoneMapping, 
		const aiMatrix4x4& globalInverseTransform);
	~Skeleton();

	MeshNode* getRootNode();
	void getNodeByName(MeshNode*& foundNode, MeshNode& currentNode, const std::string& nodeName);
	void resetAllKeyFrameIndexes();

	void setTransitionInterpolationFactor(const float interpolationFactor);
	void updateNode(MeshNode& node, const aiMatrix4x4& parentTransform, const double& animationTime);
	void readSkeletonBoneTransformations(std::vector<aiMatrix4x4>& transforms);

	void applyTransformationToNodeAndChildrenRawBoneTransform(const MeshNode& meshNode, const aiMatrix4x4& transformation);
	void blockChildNodeAndParentsFromTransformations(const std::string& childNodeName, const BlockedTransformComponents& blockedComponents);
	void unblockNodeAndChildrenTransformations(MeshNode& meshNode);

private:
	void updateBoneTransformation(const MeshNode& meshNode, const aiMatrix4x4& childTransformation);
	void interpolateNodeToFirstKeyFrameFromCurrentBoneTransform(const aiMatrix4x4& currentNodeTransform, const unsigned int boneIndex);

	void mapSkeletonToAnimation(const aiAnimation* animation);
	void constructSkeletonFromNodeTree(const aiNode* aiRootNode);
	void addChildNode(MeshNode& parentNode, const aiNode* node, const int childIndex);
	MeshNode createNode(const aiNode* node, MeshNode* parentNode);

	MeshNode* rootNode;

	float transitionInterpolationFactor;
	aiMatrix4x4 globalInverseTransform;

	const std::map<std::string, unsigned int>* sharedBoneMapping;
	std::vector<BoneInfo>* boneInfo;

	std::vector<NodeAnimation*> nodeAnimationRawStorage;
	std::unordered_map<std::string, NodeAnimation*> nodeAnimations;
};

