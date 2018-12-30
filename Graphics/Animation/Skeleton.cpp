#include "Skeleton.h"

#include "AnimationTransformHelper.h"
#include "../Meshes/Mesh.h"

Skeleton::Skeleton(const aiAnimation* animation, const aiNode* aiRootNode, const Mesh* mesh, 
	std::vector<BoneInfo>* boneInfo, const std::map<std::string, unsigned int>* sharedBoneMapping, 
	const aiMatrix4x4& globalInverseTransform)
	: sharedBoneMapping(sharedBoneMapping)
{
	this->rootNode = new MeshNode();
	this->boneInfo = boneInfo;
	this->globalInverseTransform = globalInverseTransform;

	transitionInterpolationFactor = 1.0f;

	MapSkeletonToAnimation(animation);
	ConstructSkeletonFromNodeTree(aiRootNode);
}

Skeleton::~Skeleton()
{
	for (NodeAnimation* nodeAnimation : nodeAnimationRawStorage)
	{
		delete nodeAnimation;
	}

	nodeAnimationRawStorage.clear();
	nodeAnimations.clear();
	delete rootNode;
}

MeshNode* Skeleton::GetRootNode()
{
	return rootNode;
}

void Skeleton::GetNodeByName(MeshNode*& foundNode, MeshNode& currentNode, const std::string& nodeName)
{
	if (currentNode.nodeName == nodeName)
	{
		foundNode = &currentNode;
	}
	else
	{
		for (int i = 0; i < currentNode.node->mNumChildren; ++i)
		{
			GetNodeByName(foundNode, currentNode.children[i], nodeName);
		}
	}
}

void Skeleton::ResetAllKeyFrameIndexes()
{
	for (NodeAnimation* nodeAnimation : nodeAnimationRawStorage)
	{
		nodeAnimation->ResetKeyFrameIndexes();
	}
}

void Skeleton::SetTransitionInterpolationFactor(const float interpolationFactor)
{
	transitionInterpolationFactor = interpolationFactor;
}

void Skeleton::UpdateNode(MeshNode& meshNode, const aiMatrix4x4& parentTransform, const double& animationTime)
{
	aiMatrix4x4 nodeTransformation(meshNode.node->mTransformation);

	if (meshNode.hasAnimation)
	{
		NodeAnimation* nodeAnimation = nodeAnimations.at(meshNode.nodeName);
		AnimationTransformHelper::CalculateNodeTransformation(nodeTransformation, *nodeAnimation,
			animationTime, meshNode.blockedComponents);
	}

	aiMatrix4x4 childTransformation = parentTransform * nodeTransformation;
	meshNode.rawTransform = childTransformation;

	UpdateBoneTransformation(meshNode, childTransformation);

	for (unsigned int i = 0; i < meshNode.node->mNumChildren; i++)
	{
		UpdateNode(meshNode.children[i], childTransformation, animationTime);
	}
}

void Skeleton::ReadSkeletonBoneTransformations(std::vector<aiMatrix4x4>& transforms)
{
	const int numBones = (int)boneInfo->size();
	for (unsigned int i = 0; i < numBones; i++)
	{
		transforms[i] = (*boneInfo)[i].finalTransformation;
	}
}

void Skeleton::ApplyTransformationToNodeAndChildrenRawBoneTransform(const MeshNode& meshNode, const aiMatrix4x4& transformation)
{
	if (meshNode.mapsToBone)
	{
		unsigned int boneIndex = sharedBoneMapping->at(meshNode.nodeName);
		(*boneInfo)[boneIndex].rawNodeTransform = transformation * (*boneInfo)[boneIndex].rawNodeTransform;
	}

	for (unsigned int i = 0; i < meshNode.node->mNumChildren; i++)
	{
		ApplyTransformationToNodeAndChildrenRawBoneTransform(meshNode.children[i], transformation);
	}
}

void Skeleton::BlockChildNodeAndParentsFromTransformations(const std::string & childNodeName, const BlockedTransformComponents & blockedComponents)
{
	MeshNode* childNode = nullptr;
	GetNodeByName(childNode, *rootNode, childNodeName);

	if (childNode != nullptr)
	{
		MeshNode* currentNode = childNode;

		while (currentNode != nullptr)
		{
			currentNode->blockedComponents = blockedComponents;
			currentNode = currentNode->parent;
		}
	}
}

void Skeleton::UnblockNodeAndChildrenTransformations(MeshNode& meshNode)
{
	meshNode.blockedComponents = BlockedTransformComponents();

	for (int i = 0; i < meshNode.node->mNumChildren; ++i)
	{
		UnblockNodeAndChildrenTransformations(meshNode.children[i]);
	}
}

void Skeleton::UpdateBoneTransformation(const MeshNode& meshNode, const aiMatrix4x4& childTransformation)
{
	if (meshNode.mapsToBone)
	{
		unsigned int boneIndex = sharedBoneMapping->at(meshNode.nodeName);

		if (transitionInterpolationFactor < 1.0f)
		{
			InterpolateNodeToFirstKeyFrameFromCurrentBoneTransform(childTransformation, boneIndex);
		}
		else
		{
			(*boneInfo)[boneIndex].finalTransformation = globalInverseTransform * childTransformation *
				(*boneInfo)[boneIndex].boneOffset;
			(*boneInfo)[boneIndex].rawNodeTransform = childTransformation;
		}
	}
}

void Skeleton::InterpolateNodeToFirstKeyFrameFromCurrentBoneTransform(const aiMatrix4x4& currentNodeTransform, const unsigned int boneIndex)
{
	DecomposedMatrix decomposedPreviousTransform;
	DecomposedMatrix decomposedCurrentTransform;

	(*boneInfo)[boneIndex].rawNodeTransform.Decompose(decomposedPreviousTransform.scale, decomposedPreviousTransform.rotation,
		decomposedPreviousTransform.translation);
	currentNodeTransform.Decompose(decomposedCurrentTransform.scale, decomposedCurrentTransform.rotation,
		decomposedCurrentTransform.translation);

	DecomposedMatrix decomposedInterpolatedTransform;
	AnimationTransformHelper::InterpolateDecomposedMatrices(decomposedInterpolatedTransform, decomposedPreviousTransform,
		decomposedCurrentTransform, transitionInterpolationFactor);

	aiMatrix4x4 interpolatedTransform;
	AnimationTransformHelper::ComposeMatrix(interpolatedTransform, decomposedInterpolatedTransform);

	(*boneInfo)[boneIndex].finalTransformation = globalInverseTransform * interpolatedTransform *
		(*boneInfo)[boneIndex].boneOffset;
}

void Skeleton::MapSkeletonToAnimation(const aiAnimation* animation)
{
	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		nodeAnimationRawStorage.push_back(new NodeAnimation(animation->mChannels[i]));
		nodeAnimations.insert({ std::string(animation->mChannels[i]->mNodeName.data), nodeAnimationRawStorage.back() });
	}
}

void Skeleton::ConstructSkeletonFromNodeTree(const aiNode* aiRootNode)
{
	*rootNode = CreateNode(aiRootNode, nullptr);

	for (unsigned int i = 0; i < aiRootNode->mNumChildren; i++)
	{
		AddChildNode(*rootNode, aiRootNode->mChildren[i], i);
	}
}

void Skeleton::AddChildNode(MeshNode& parentNode, const aiNode* node, const int childIndex)
{
	MeshNode childNode = CreateNode(node, &parentNode);
	parentNode.children[childIndex] = childNode;

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		AddChildNode(parentNode.children[childIndex], node->mChildren[i], i);
	}
}

MeshNode Skeleton::CreateNode(const aiNode* node, MeshNode* parentNode)
{
	MeshNode childNode;
	childNode.node = node;
	childNode.nodeName = string(node->mName.data);
	childNode.hasAnimation = nodeAnimations.find(node->mName.data) != nodeAnimations.end();
	childNode.mapsToBone = sharedBoneMapping->find(node->mName.data) != sharedBoneMapping->end();
	childNode.children = new MeshNode[node->mNumChildren];
	childNode.parent = parentNode;

	return childNode;
}
