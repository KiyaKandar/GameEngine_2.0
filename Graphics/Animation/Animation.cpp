#include "Animation.h"

#include "AnimationTransformHelper.h"
#include "AnimationComponents.h"
#include "../Meshes/Mesh.h"

#include <map>

struct MeshNode
{
	const aiNode* node;
	MeshNode* children;
	MeshNode* parent;
	std::string nodeName;

	bool mapsToBone = false;
	bool hasAnimation = false;
	BlockedTransformComponents blockedComponents;
};

Animation::Animation(const std::string& animationName, const std::string& gameObjectId, Mesh* mesh, const aiAnimation* animation,
	const aiNode* rootNode, const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo)
	: animationId(Hash{}(animationName))
	, owningGameObjectId(Hash{}(gameObjectId))
	, mesh(mesh)
	, globalInverseTransform(globalInverseTransform)
{
	this->animation = animation;
	boneInfo = initialBoneInfo;

	this->rootNode = new MeshNode();

	reset();

	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		nodeAnimationRawStorage.push_back(new NodeAnimation(animation->mChannels[i]));
		nodeAnimations.insert({ string(animation->mChannels[i]->mNodeName.data), nodeAnimationRawStorage.back() });
	}

	constructNodeList(rootNode);
}

Animation::~Animation()
{
	for (NodeAnimation* nodeAnimation : nodeAnimationRawStorage)
	{
		delete nodeAnimation;
	}

	nodeAnimationRawStorage.clear();
	nodeAnimations.clear();
	delete rootNode;
}

bool Animation::hasGameObjectIdMatchOnly(const size_t& gameObjectId) const
{
	return owningGameObjectId == gameObjectId;
}

bool Animation::hasAnimationIdMatchOnly(const size_t & animationId) const
{
	return this->animationId == animationId;
}

bool Animation::hasIdMatch(const size_t& meshId, const size_t& animationId) const
{
	return hasGameObjectIdMatchOnly(meshId) && hasAnimationIdMatchOnly(animationId);
}

bool Animation::isLooping() const
{
	return looping;
}

void Animation::incrementTimer(const double& deltaTime)
{
	const double deltaTimeInSeconds = deltaTime * 0.001f;

	if (remainingLerpDurationFromPreviousTransformation > 0.0)
	{
		remainingLerpDurationFromPreviousTransformation -= deltaTimeInSeconds;
		interpolationFactor = 1.0f - (float)(remainingLerpDurationFromPreviousTransformation
			/ totalLerpDurationFromPreviousTransformation);
	}

	elapsedTime += deltaTimeInSeconds;
}

void Animation::reset()
{
	elapsedTime = 0.0;
	animationTime = 0.0;
	totalLerpDurationFromPreviousTransformation = 0.0;
	remainingLerpDurationFromPreviousTransformation = 0.0;
	interpolationFactor = 0.0f;

	ResetAllKeyFrameIndexes();
}

void Animation::setDurationToLerpFromPreviousAniamtion(const double& lerpDuration)
{
	totalLerpDurationFromPreviousTransformation = lerpDuration;
	remainingLerpDurationFromPreviousTransformation = lerpDuration;
}

void Animation::setLooping(const bool looping)
{
	this->looping = looping;
}

void Animation::blockTransformationForNode(const std::string& nodeName, const BlockedTransformComponents& blockedComponents)
{
	if (nodeName != "")
	{
		searchChildNodeToBlockNodeTransformation(*rootNode, nodeName, blockedComponents);
	}
	else
	{
		unblockChildNodeTransformation(*rootNode);
	}
}

bool Animation::finishedPlaying() const
{
	if (looping)
	{
		return true;
	}
	else
	{
		return elapsedTime * animation->mTicksPerSecond >= animation->mDuration;
	}
}

bool Animation::meshIsOnScreen() const
{
	return mesh->onScreen;
}

void Animation::updateAnimationTransformState()
{
	std::vector<aiMatrix4x4> nextState;
	nextState.resize(mesh->numBones);
	transformBones(nextState);
	animationState = nextState;
}

void Animation::readAnimationState(std::vector<aiMatrix4x4>& animationState) const
{
	animationState = this->animationState;
}

void Animation::validateLastKeyFrames(const double timeInTicks)
{
	if (timeInTicks >= animation->mDuration)
	{
		ResetAllKeyFrameIndexes();
	}
}

void Animation::ResetAllKeyFrameIndexes()
{
	for (NodeAnimation* nodeAnimation : nodeAnimationRawStorage)
	{
		nodeAnimation->ResetKeyFrameIndexes();
	}
}

void Animation::constructNodeList(const aiNode* aiRootNode)
{
	rootNode->node = aiRootNode;
	rootNode->nodeName = string(aiRootNode->mName.data);
	rootNode->hasAnimation = nodeAnimations.find(aiRootNode->mName.data) != nodeAnimations.end();
	rootNode->mapsToBone = mesh->boneMapping.find(aiRootNode->mName.data) != mesh->boneMapping.end();
	rootNode->children = new MeshNode[aiRootNode->mNumChildren];
	rootNode->parent = nullptr;

	for (unsigned int i = 0; i < aiRootNode->mNumChildren; i++)
	{
		addNode(*rootNode, aiRootNode->mChildren[i], i);
	}
}

void Animation::addNode(MeshNode& parentNode, const aiNode* node, const int childIndex)
{
	MeshNode childNode;
	childNode.node = node;
	childNode.nodeName = string(node->mName.data);
	childNode.hasAnimation = nodeAnimations.find(node->mName.data) != nodeAnimations.end();
	childNode.mapsToBone = mesh->boneMapping.find(node->mName.data) != mesh->boneMapping.end();
	childNode.children = new MeshNode[node->mNumChildren];
	childNode.parent = &parentNode;

	parentNode.children[childIndex] = childNode;

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		addNode(parentNode.children[childIndex], node->mChildren[i], i);
	}
}

void Animation::searchChildNodeToBlockNodeTransformation(MeshNode& childNode, const std::string& nodeName, const BlockedTransformComponents& blockedComponents)
{
	if (childNode.nodeName == nodeName)
	{
		MeshNode* currentNode = &childNode;

		while (currentNode)
		{
			currentNode->blockedComponents = blockedComponents;
			currentNode = currentNode->parent;
		}
	}
	else
	{
		for (int i = 0; i < childNode.node->mNumChildren; ++i)
		{
			searchChildNodeToBlockNodeTransformation(childNode.children[i], nodeName, blockedComponents);
		}
	}
}

void Animation::unblockChildNodeTransformation(MeshNode& childNode)
{
	childNode.blockedComponents = BlockedTransformComponents();

	for (int i = 0; i < childNode.node->mNumChildren; ++i)
	{
		unblockChildNodeTransformation(childNode.children[i]);
	}
}

void Animation::transformBones(std::vector<aiMatrix4x4>& transforms)
{
	const double ticksPerSecond = animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f;
	const double timeInTicks = elapsedTime * ticksPerSecond;

	validateLastKeyFrames(timeInTicks);
	animationTime = fmod(timeInTicks, animation->mDuration);

	updateNode(*rootNode, aiMatrix4x4());

	for (unsigned int i = 0; i < mesh->numBones; i++)
	{
		transforms[i] = (*boneInfo)[i].finalTransformation;
	}
}

void Animation::updateNode(const MeshNode& meshNode, const aiMatrix4x4& parentTransform)
{
	aiMatrix4x4 nodeTransformation(meshNode.node->mTransformation);

	if (meshNode.hasAnimation)
	{
		NodeAnimation* nodeAnimation = nodeAnimations.at(meshNode.nodeName);
		AnimationTransformHelper::calculateNodeTransformation(nodeTransformation, *nodeAnimation, 
			animationTime, meshNode.blockedComponents);
	}

	aiMatrix4x4 childTransformation = parentTransform * nodeTransformation;
	updateBoneTransformation(meshNode, childTransformation);

	for (unsigned int i = 0; i < meshNode.node->mNumChildren; i++)
	{
		updateNode(meshNode.children[i], childTransformation);
	}
}

void Animation::updateBoneTransformation(const MeshNode& meshNode, const aiMatrix4x4& childTransformation)
{
	if (meshNode.mapsToBone)
	{
		unsigned int boneIndex = mesh->boneMapping.at(meshNode.nodeName);

		if (remainingLerpDurationFromPreviousTransformation > 0.0)
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

void Animation::interpolateNodeToFirstKeyFrameFromCurrentBoneTransform(const aiMatrix4x4& currentNodeTransform, const unsigned int boneIndex)
{
	DecomposedMatrix decomposedPreviousTransform;
	DecomposedMatrix decomposedCurrentTransform;

	(*boneInfo)[boneIndex].rawNodeTransform.Decompose(decomposedPreviousTransform.scale, decomposedPreviousTransform.rotation,
		decomposedPreviousTransform.translation);
	currentNodeTransform.Decompose(decomposedCurrentTransform.scale, decomposedCurrentTransform.rotation, 
		decomposedCurrentTransform.translation);

	DecomposedMatrix decomposedInterpolatedTransform;
	AnimationTransformHelper::interpolateDecomposedMatrices(decomposedInterpolatedTransform, decomposedPreviousTransform, 
		decomposedCurrentTransform, interpolationFactor);

	aiMatrix4x4 interpolatedTransform;
	AnimationTransformHelper::composeMatrix(interpolatedTransform, decomposedInterpolatedTransform);

	(*boneInfo)[boneIndex].finalTransformation = globalInverseTransform * interpolatedTransform *
		(*boneInfo)[boneIndex].boneOffset;
}
