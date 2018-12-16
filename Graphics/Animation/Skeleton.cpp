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

	mapSkeletonToAnimation(animation);
	constructSkeletonFromNodeTree(aiRootNode);
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

MeshNode* Skeleton::getRootNode()
{
	return rootNode;
}

void Skeleton::getNodeByName(MeshNode*& foundNode, MeshNode& currentNode, const std::string& nodeName)
{
	if (currentNode.nodeName == nodeName)
	{
		foundNode = &currentNode;
	}
	else
	{
		for (int i = 0; i < currentNode.node->mNumChildren; ++i)
		{
			getNodeByName(foundNode, currentNode.children[i], nodeName);
		}
	}
}

void Skeleton::resetAllKeyFrameIndexes()
{
	for (NodeAnimation* nodeAnimation : nodeAnimationRawStorage)
	{
		nodeAnimation->ResetKeyFrameIndexes();
	}
}

void Skeleton::setTransitionInterpolationFactor(const float interpolationFactor)
{
	transitionInterpolationFactor = interpolationFactor;
}

void Skeleton::updateNode(MeshNode& meshNode, const aiMatrix4x4& parentTransform, const double& animationTime)
{
	aiMatrix4x4 nodeTransformation(meshNode.node->mTransformation);

	if (meshNode.hasAnimation)
	{
		NodeAnimation* nodeAnimation = nodeAnimations.at(meshNode.nodeName);
		AnimationTransformHelper::calculateNodeTransformation(nodeTransformation, *nodeAnimation,
			animationTime, meshNode.blockedComponents);
	}

	aiMatrix4x4 childTransformation = parentTransform * nodeTransformation;
	meshNode.rawTransform = childTransformation;

	updateBoneTransformation(meshNode, childTransformation);

	for (unsigned int i = 0; i < meshNode.node->mNumChildren; i++)
	{
		updateNode(meshNode.children[i], childTransformation, animationTime);
	}
}

void Skeleton::readSkeletonBoneTransformations(std::vector<aiMatrix4x4>& transforms)
{
	const int numBones = (int)boneInfo->size();
	for (unsigned int i = 0; i < numBones; i++)
	{
		transforms[i] = (*boneInfo)[i].finalTransformation;
	}
}

void Skeleton::applyTransformationToNodeAndChildrenRawBoneTransform(const MeshNode& meshNode, const aiMatrix4x4& transformation)
{
	if (meshNode.mapsToBone)
	{
		unsigned int boneIndex = sharedBoneMapping->at(meshNode.nodeName);
		(*boneInfo)[boneIndex].rawNodeTransform = transformation * (*boneInfo)[boneIndex].rawNodeTransform;
	}

	for (unsigned int i = 0; i < meshNode.node->mNumChildren; i++)
	{
		applyTransformationToNodeAndChildrenRawBoneTransform(meshNode.children[i], transformation);
	}
}

void Skeleton::blockChildNodeAndParentsFromTransformations(const std::string & childNodeName, const BlockedTransformComponents & blockedComponents)
{
	MeshNode* childNode = nullptr;
	getNodeByName(childNode, *rootNode, childNodeName);

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

void Skeleton::unblockNodeAndChildrenTransformations(MeshNode& meshNode)
{
	meshNode.blockedComponents = BlockedTransformComponents();

	for (int i = 0; i < meshNode.node->mNumChildren; ++i)
	{
		unblockNodeAndChildrenTransformations(meshNode.children[i]);
	}
}

void Skeleton::updateBoneTransformation(const MeshNode& meshNode, const aiMatrix4x4& childTransformation)
{
	if (meshNode.mapsToBone)
	{
		unsigned int boneIndex = sharedBoneMapping->at(meshNode.nodeName);

		if (transitionInterpolationFactor < 1.0f)
		{
			interpolateNodeToFirstKeyFrameFromCurrentBoneTransform(childTransformation, boneIndex);
		}
		else
		{
			(*boneInfo)[boneIndex].finalTransformation = globalInverseTransform * childTransformation *
				(*boneInfo)[boneIndex].boneOffset;
			(*boneInfo)[boneIndex].rawNodeTransform = childTransformation;
		}
	}
}

void Skeleton::interpolateNodeToFirstKeyFrameFromCurrentBoneTransform(const aiMatrix4x4& currentNodeTransform, const unsigned int boneIndex)
{
	DecomposedMatrix decomposedPreviousTransform;
	DecomposedMatrix decomposedCurrentTransform;

	(*boneInfo)[boneIndex].rawNodeTransform.Decompose(decomposedPreviousTransform.scale, decomposedPreviousTransform.rotation,
		decomposedPreviousTransform.translation);
	currentNodeTransform.Decompose(decomposedCurrentTransform.scale, decomposedCurrentTransform.rotation,
		decomposedCurrentTransform.translation);

	DecomposedMatrix decomposedInterpolatedTransform;
	AnimationTransformHelper::interpolateDecomposedMatrices(decomposedInterpolatedTransform, decomposedPreviousTransform,
		decomposedCurrentTransform, transitionInterpolationFactor);

	aiMatrix4x4 interpolatedTransform;
	AnimationTransformHelper::composeMatrix(interpolatedTransform, decomposedInterpolatedTransform);

	(*boneInfo)[boneIndex].finalTransformation = globalInverseTransform * interpolatedTransform *
		(*boneInfo)[boneIndex].boneOffset;
}

void Skeleton::mapSkeletonToAnimation(const aiAnimation* animation)
{
	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		nodeAnimationRawStorage.push_back(new NodeAnimation(animation->mChannels[i]));
		nodeAnimations.insert({ std::string(animation->mChannels[i]->mNodeName.data), nodeAnimationRawStorage.back() });
	}
}

void Skeleton::constructSkeletonFromNodeTree(const aiNode* aiRootNode)
{
	*rootNode = createNode(aiRootNode, nullptr);

	for (unsigned int i = 0; i < aiRootNode->mNumChildren; i++)
	{
		addChildNode(*rootNode, aiRootNode->mChildren[i], i);
	}
}

void Skeleton::addChildNode(MeshNode& parentNode, const aiNode* node, const int childIndex)
{
	MeshNode childNode = createNode(node, &parentNode);
	parentNode.children[childIndex] = childNode;

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		addChildNode(parentNode.children[childIndex], node->mChildren[i], i);
	}
}

MeshNode Skeleton::createNode(const aiNode* node, MeshNode* parentNode)
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
