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

	MeshNode* GetRootNode();
	void GetNodeByName(MeshNode*& foundNode, MeshNode& currentNode, const std::string& nodeName);
	void ResetAllKeyFrameIndexes();

	void SetTransitionInterpolationFactor(const float interpolationFactor);
	void UpdateNode(MeshNode& node, const aiMatrix4x4& parentTransform, const double& animationTime);
	void ReadSkeletonBoneTransformations(std::vector<aiMatrix4x4>& transforms);

	void ApplyTransformationToNodeAndChildrenRawBoneTransform(const MeshNode& meshNode, const aiMatrix4x4& transformation);
	void BlockChildNodeAndParentsFromTransformations(const std::string& childNodeName, const BlockedTransformComponents& blockedComponents);
	void UnblockNodeAndChildrenTransformations(MeshNode& meshNode);

private:
	void UpdateBoneTransformation(const MeshNode& meshNode, const aiMatrix4x4& childTransformation);
	void InterpolateNodeToFirstKeyFrameFromCurrentBoneTransform(const aiMatrix4x4& currentNodeTransform, const unsigned int boneIndex);

	void MapSkeletonToAnimation(const aiAnimation* animation);
	void ConstructSkeletonFromNodeTree(const aiNode* aiRootNode);
	void AddChildNode(MeshNode& parentNode, const aiNode* node, const int childIndex);
	MeshNode CreateNode(const aiNode* node, MeshNode* parentNode);

	MeshNode* rootNode;

	float transitionInterpolationFactor;
	aiMatrix4x4 globalInverseTransform;

	const std::map<std::string, unsigned int>* sharedBoneMapping;
	std::vector<BoneInfo>* boneInfo;

	std::vector<NodeAnimation*> nodeAnimationRawStorage;
	std::unordered_map<std::string, NodeAnimation*> nodeAnimations;
};

