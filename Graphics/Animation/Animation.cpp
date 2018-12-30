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

	Reset();
}

Animation::~Animation()
{
	delete skeleton;
	delete debugSkeletonDisplay;
}

const std::string Animation::GetOwningGameObjectName() const
{
	return owningGameObjectName;
}

bool Animation::HasGameObjectIdMatchOnly(const size_t& gameObjectId) const
{
	return owningGameObjectId == gameObjectId;
}

bool Animation::HasAnimationIdMatchOnly(const size_t & animationId) const
{
	return this->animationId == animationId;
}

bool Animation::HasIdMatch(const size_t& meshId, const size_t& animationId) const
{
	return HasGameObjectIdMatchOnly(meshId) && HasAnimationIdMatchOnly(animationId);
}

bool Animation::IsLooping() const
{
	return looping;
}

void Animation::UpdateSceneNodeTransformFromNode(const NodeTransformSpecifier& nodeSpecifier)
{
	MeshNode* foundNode = nullptr;
	skeleton->GetNodeByName(foundNode, *skeleton->GetRootNode(), nodeSpecifier.nodeName);

	aiMatrix4x4 sceneNodeRelativeTransformation;
	AnimationTransformHelper::RemoveBlockedComponentsFromTransform(sceneNodeRelativeTransformation, 
		foundNode->rawTransform, nodeSpecifier.blockedComponents);

	RelativeTransformMessage message("RenderingSystem", owningGameObjectName, 
		NCLMatrix4(sceneNodeRelativeTransformation));
	DeliverySystem::GetPostman()->InsertMessage(message);

	RemoveSceneNodeTransformFromBones(*foundNode, sceneNodeRelativeTransformation, nodeSpecifier.blockedComponents);
}

aiMatrix4x4 Animation::GetCurrentTransformOfSceneNodeTransformerNode(const std::string nodeName)
{
	if (nodeName == "")
	{
		return skeleton->GetRootNode()->rawTransform;
	}
	else
	{
		MeshNode* foundNode = nullptr;
		skeleton->GetNodeByName(foundNode, *skeleton->GetRootNode(), nodeName);
		return foundNode->rawTransform;
	}
}

void Animation::DebugDrawSkeleton(const aiMatrix4x4& parentTransform)
{
	debugSkeletonDisplay->DrawSkeleton(*skeleton->GetRootNode(), parentTransform);
}

void Animation::SetBlockedSkeletonNodeTransforms(const std::string& nodeName, const BlockedTransformComponents& blockedComponents)
{
	if (nodeName != "")
	{
		skeleton->BlockChildNodeAndParentsFromTransformations(nodeName, blockedComponents);
	}
	else
	{
		skeleton->UnblockNodeAndChildrenTransformations(*skeleton->GetRootNode());
	}
}

void Animation::IncrementTimer(const double& deltaTime)
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

void Animation::Reset()
{
	elapsedTime = 0.0;
	animationTime = 0.0;
	totalLerpDurationFromPreviousTransformation = 0.0;
	remainingLerpDurationFromPreviousTransformation = 0.0;
	interpolationFactor = 0.0f;
	skeleton->ResetAllKeyFrameIndexes();
}

void Animation::SetDurationToLerpFromPreviousAniamtion(const double& lerpDuration)
{
	totalLerpDurationFromPreviousTransformation = lerpDuration;
	remainingLerpDurationFromPreviousTransformation = lerpDuration;
}

void Animation::SetLooping(const bool looping)
{
	this->looping = looping;
}

bool Animation::FinishedPlaying() const
{
	return elapsedTime * animation->mTicksPerSecond >= animation->mDuration;
}

bool Animation::MeshIsOnScreen() const
{
	return mesh->onScreen;
}

void Animation::UpdateAnimationTransformState()
{
	std::vector<aiMatrix4x4> nextState;
	nextState.resize(mesh->numBones);
	TransformBones(nextState);
	animationState = nextState;
}

void Animation::ReadAnimationState(std::vector<aiMatrix4x4>& animationState) const
{
	animationState = this->animationState;
}

void Animation::ValidateLastKeyFrames(const double timeInTicks)
{
	if (timeInTicks >= animation->mDuration)
	{
		skeleton->ResetAllKeyFrameIndexes();
	}
}

void Animation::TransformBones(std::vector<aiMatrix4x4>& transforms)
{
	const double ticksPerSecond = animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f;
	const double timeInTicks = elapsedTime * ticksPerSecond;

	ValidateLastKeyFrames(timeInTicks);
	animationTime = fmod(timeInTicks, animation->mDuration);

	skeleton->SetTransitionInterpolationFactor(interpolationFactor);
	skeleton->UpdateNode(*skeleton->GetRootNode(), aiMatrix4x4(), animationTime);
	skeleton->ReadSkeletonBoneTransformations(transforms);
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
	AnimationTransformHelper::ComposeMatrix(revertedMatrix, a, blockedComponents);

	skeleton->ApplyTransformationToNodeAndChildrenRawBoneTransform(node, revertedMatrix);
}
