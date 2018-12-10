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

	QueuedAnimation() = delete;

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

void AnimationManager::addAnimation(const std::string& animationName, const std::string& gameObjectId, Mesh* mesh, const aiAnimation* animation, 
	const aiNode* rootNode, const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo)
{
	animations.push_back(new Animation(animationName, gameObjectId, mesh, animation, rootNode, globalInverseTransform, initialBoneInfo));
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

void AnimationManager::readAnimationStateForSceneNode(const std::string& gameObjectId, std::vector<aiMatrix4x4>& animationStates) const
{
	const size_t id = Hash{}(gameObjectId);

	for (const Animation* animation : activeAnimations)
	{
		if (animation->hasGameObjectIdMatchOnly(id))
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
			const size_t gameObjectId = Hash{}(queuedAnimation.gameObjectId);
			const size_t animationId = Hash{}(queuedAnimation.animationName);
			const bool alreadyPlaying = RemoveActiveAnimation(gameObjectId, animationId);

			if (!alreadyPlaying)
			{
				BeginPlayingAnimation(gameObjectId, animationId, queuedAnimation.lerpToTime);
			}
		}

		animationsToAddtoPlayQueue.clear();
	}
}

bool AnimationManager::RemoveActiveAnimation(const size_t& gameObjectId, const size_t& animationId)
{
	std::vector<Animation*>::iterator animationIterator;
	for (animationIterator = activeAnimations.begin(); animationIterator != activeAnimations.end(); ++animationIterator)
	{
		if ((*animationIterator)->hasGameObjectIdMatchOnly(gameObjectId))
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

void AnimationManager::BeginPlayingAnimation(const size_t& gameObjectId, const size_t& animationId, const double lerpToTime)
{
	for (Animation* animation : animations)
	{
		if (animation->hasIdMatch(gameObjectId, animationId))
		{
			activeAnimations.push_back(animation);
			animation->SetDurationToLerpFromPreviousAniamtion(lerpToTime);
			break;
		}
	}
}
