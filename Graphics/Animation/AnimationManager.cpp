#include "AnimationManager.h"

#include "Animation.h"
#include "AnimationComponents.h"
#include "../Utilities/GameTimer.h"
#include "../../Communication/Messages/PlayAnimationMessage.h"
#include "../../Resource Management/Database/Database.h"

using namespace std::placeholders;

struct QueuedAnimation
{
	QueuedAnimation(std::string gameObjectId, std::string animationName, double lerpToTime)
	{
		this->gameObjectId = gameObjectId;
		this->animationName = animationName;
		this->lerpToTime = lerpToTime;
	}

	std::string gameObjectId;
	std::string animationName;
	double lerpToTime;
};

AnimationManager::AnimationManager(Database* database)
	: Subsystem("AnimationManager")
{
	this->database = database;

	incomingMessages = MessageProcessor(std::vector<MessageType> { MessageType::PLAY_ANIMATION },
		DeliverySystem::getPostman()->getDeliveryPoint("AnimationManager"));

	incomingMessages.addActionToExecuteOnMessage(MessageType::PLAY_ANIMATION, std::bind(&AnimationManager::QueueAnimationPlay, this, _1));
}

AnimationManager::~AnimationManager()
{
	clearAnimations();
}

void AnimationManager::updateNextFrame(const float& deltaTime)
{
	timer->beginTimedSection();

	ActivateAnimationsInPlayQueue();

	for (Animation* animation : activeAnimations)
	{
		animation->incrementTimer((double)deltaTime);

		if (animation->meshIsOnScreen())
		{
			animation->updateAnimationTransformState();
		}
	}

	timer->endTimedSection();
}

void AnimationManager::QueueAnimationPlay(Message* message)
{
	PlayAnimationMessage* playMessage = static_cast<PlayAnimationMessage*>(message);
	animationsToAddtoPlayQueue.push_back(QueuedAnimation(playMessage->gameObjectID, playMessage->animationName, playMessage->lerpToTime));
}

void AnimationManager::addAnimation(const std::string& animationName, Mesh* mesh, const aiAnimation* animation, const aiNode* rootNode, const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo)
{
	animations.push_back(new Animation(animationName, mesh, animation, rootNode, globalInverseTransform, initialBoneInfo));
}

void AnimationManager::clearAnimations()
{
	for (Animation* animation : animations)
	{
		delete animation;
	}

	animations.clear();
	activeAnimations.clear();
	animationsToAddtoPlayQueue.clear();
}

void AnimationManager::readAnimationStateForMesh(const std::string& meshName, std::vector<aiMatrix4x4>& animationStates) const
{
	const size_t id = Hash{}(meshName);

	for (const Animation* animation : activeAnimations)
	{
		if (animation->hasMeshIdMatchOnly(id))
		{
			animation->readAnimationState(animationStates);
			break;
		}
	}
}

void AnimationManager::ActivateAnimationsInPlayQueue()
{
	if (!animationsToAddtoPlayQueue.empty())
	{
		for (QueuedAnimation& queuedAnimation : animationsToAddtoPlayQueue)
		{
			Resource* resource = database->getTable("GameObjects")->getResource(queuedAnimation.gameObjectId);
			GameObject* gameObject = static_cast<GameObject*>(resource);

			const size_t meshId = Hash{}(gameObject->getSceneNode()->GetMesh()->getName());
			const size_t animationId = Hash{}(queuedAnimation.animationName);
			const bool alreadyPlaying = RemoveActiveMeshAnimation(meshId, animationId);

			if (!alreadyPlaying)
			{
				BeginPlayingAnimation(meshId, animationId, queuedAnimation.lerpToTime);
			}
		}

		animationsToAddtoPlayQueue.clear();
	}
}

bool AnimationManager::RemoveActiveMeshAnimation(const size_t& meshId, const size_t& animationId)
{
	std::vector<Animation*>::iterator animationIterator;
	for (animationIterator = activeAnimations.begin(); animationIterator != activeAnimations.end(); ++animationIterator)
	{
		if ((*animationIterator)->hasMeshIdMatchOnly(meshId))
		{
			if (!(*animationIterator)->hasAnimationIdMatchOnly(animationId))
			{
				//Remove mesh's active animation
				activeAnimations.erase(animationIterator);
			}
			else
			{
				//Already playing
				return true;
			}

			return false;
		}
	}

	return false;
}

void AnimationManager::BeginPlayingAnimation(const size_t& meshId, const size_t& animationId, const double lerpToTime)
{
	for (Animation* animation : animations)
	{
		if (animation->hasIdMatch(meshId, animationId))
		{
			activeAnimations.push_back(animation);
			animation->SetDurationToLerpFromPreviousAniamtion(lerpToTime);
			break;
		}
	}
}
