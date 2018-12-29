#include "Animation.h"

#include "AnimationTransformHelper.h"
#include "AnimationComponents.h"
#include "SkeletonDisplay.h"
#include "../Meshes/Mesh.h"
#include "../../Communication/DeliverySystem.h"

Animation::Animation(const std::string& animationName, const std::string& gameObjectId, Mesh* mesh, const aiAnimation* animation,
	const aiNode* rootNode, const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo)
	: animationId(Hash{}(animationName))
	, owningGameObjectId(Hash{}(gameObjectId))
	, owningGameObjectName(gameObjectId)
	, mesh(mesh)
{
	this->animation = animation;

	skeleton = new Skeleton(animation, rootNode, mesh, initialBoneInfo, &mesh->boneMapping, globalInverseTransform);
	debugSkeletonDisplay = new SkeletonDisplay();

	reset();
}

Animation::~Animation()
{
	delete skeleton;
	delete debugSkeletonDisplay;
}

const std::string Animation::getOwningGameObjectName() const
{
	return owningGameObjectName;
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

void Animation::updateSceneNodeTransformFromNode(const NodeTransformSpecifier& nodeSpecifier)
{
	MeshNode* foundNode = nullptr;
	skeleton->getNodeByName(foundNode, *skeleton->getRootNode(), nodeSpecifier.nodeName);

	aiMatrix4x4 sceneNodeRelativeTransformation;
	AnimationTransformHelper::removeBlockedComponentsFromTransform(sceneNodeRelativeTransformation, 
		foundNode->rawTransform, nodeSpecifier.blockedComponents);

	RelativeTransformMessage message("RenderingSystem", owningGameObjectName, 
		NCLMatrix4(sceneNodeRelativeTransformation));
	DeliverySystem::getPostman()->insertMessage(message);

	RemoveSceneNodeTransformFromBones(*foundNode, sceneNodeRelativeTransformation, nodeSpecifier.blockedComponents);
}

aiMatrix4x4 Animation::getCurrentTransformOfSceneNodeTransformerNode(const std::string nodeName)
{
	if (nodeName == "")
	{
		return skeleton->getRootNode()->rawTransform;
	}
	else
	{
		MeshNode* foundNode = nullptr;
		skeleton->getNodeByName(foundNode, *skeleton->getRootNode(), nodeName);
		return foundNode->rawTransform;
	}
}

void Animation::debugDrawSkeleton(const aiMatrix4x4& parentTransform)
{
	debugSkeletonDisplay->drawSkeleton(*skeleton->getRootNode(), parentTransform);
}

void Animation::setBlockedSkeletonNodeTransforms(const std::string& nodeName, const BlockedTransformComponents& blockedComponents)
{
	if (nodeName != "")
	{
		skeleton->blockChildNodeAndParentsFromTransformations(nodeName, blockedComponents);
	}
	else
	{
		skeleton->unblockNodeAndChildrenTransformations(*skeleton->getRootNode());
	}
}

void Animation::incrementTimer(const double& deltaTime)
{
	const double deltaTimeInSeconds = deltaTime * 0.001f;
	interpolationFactor = 1.0f;

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
	skeleton->resetAllKeyFrameIndexes();
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

bool Animation::finishedPlaying() const
{
	return elapsedTime * animation->mTicksPerSecond >= animation->mDuration;
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
		skeleton->resetAllKeyFrameIndexes();
	}
}

void Animation::transformBones(std::vector<aiMatrix4x4>& transforms)
{
	const double ticksPerSecond = animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f;
	const double timeInTicks = elapsedTime * ticksPerSecond;

	validateLastKeyFrames(timeInTicks);
	animationTime = fmod(timeInTicks, animation->mDuration);

	skeleton->setTransitionInterpolationFactor(interpolationFactor);
	skeleton->updateNode(*skeleton->getRootNode(), aiMatrix4x4(), animationTime);
	skeleton->readSkeletonBoneTransformations(transforms);
}

void Animation::RemoveSceneNodeTransformFromBones(const MeshNode& node, const aiMatrix4x4& sceneNodeTransform, 
	const BlockedTransformComponents& blockedComponents)
{
	DecomposedMatrix a;
	sceneNodeTransform.Decompose(a.scale, a.rotation, a.translation);

	a.translation = a.translation * -1.0f;
	a.rotation = aiQuaternion(a.rotation.x * -1.0f, a.rotation.y * -1.0f, 
		a.rotation.z * -1.0f, a.rotation.w * -1.0f);
	a.scale = a.scale * -1.0f; //TEMP

	aiMatrix4x4 revertedMatrix;
	AnimationTransformHelper::composeMatrix(revertedMatrix, a, blockedComponents);

	skeleton->applyTransformationToNodeAndChildrenRawBoneTransform(node, revertedMatrix);
}
