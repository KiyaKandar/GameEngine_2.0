#include "AnimationManager.h"

#include "Animation.h"
#include "../Utilities/GameTimer.h"

AnimationManager::AnimationManager()
	: Subsystem("AnimationManager")
{
	incomingMessages = MessageProcessor(std::vector<MessageType> { MessageType::TEXT }, // for now
		DeliverySystem::getPostman()->getDeliveryPoint("AnimationManager"));
}

AnimationManager::~AnimationManager()
{
	clearAnimations();
}

void AnimationManager::updateNextFrame(const float& deltaTime)
{
	timer->beginTimedSection();

	for (Animation* animation : animations)
	{
		animation->incrementTimer((double)deltaTime);

		if (animation->meshIsOnScreen())
		{
			animation->updateAnimationTransformState();
		}
	}

	timer->endTimedSection();
}

void AnimationManager::addAnimation(Mesh* mesh, const aiAnimation* animation, const aiNode* rootNode, const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo)
{
	animations.push_back(new Animation(mesh, animation, rootNode, globalInverseTransform, initialBoneInfo));
}

void AnimationManager::clearAnimations()
{
	for (Animation* animation : animations)
	{
		delete animation;
	}

	animations.clear();
}

void AnimationManager::readAnimationStateForMesh(const std::string& meshName, std::vector<aiMatrix4x4>& animationStates) const
{
	const size_t id = std::hash<std::string>{}(meshName);

	for (const Animation* animation : animations)
	{
		if (animation->hasIdMatch(id))
		{
			animation->readAnimationState(animationStates);
			break;
		}
	}
}
