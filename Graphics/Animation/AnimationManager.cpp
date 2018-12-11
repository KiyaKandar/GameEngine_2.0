#include "AnimationManager.h"

#include "Animation.h"
#include "AnimationComponents.h"
#include "../Utilities/GameTimer.h"
#include "../../Communication/Messages/PlayAnimationMessage.h"
#include "../../Resource Management/Database/Database.h"

using namespace std::placeholders;

AnimationManager::AnimationManager(Database* database)
	: Subsystem("AnimationManager")
{
	this->database = database;

	incomingMessages = MessageProcessor(std::vector<MessageType> { MessageType::PLAY_ANIMATION },
		DeliverySystem::getPostman()->getDeliveryPoint("AnimationManager"));

	incomingMessages.addActionToExecuteOnMessage(MessageType::PLAY_ANIMATION, std::bind(&AnimationManager::queueAnimationPlay, this, _1));
}

AnimationManager::~AnimationManager()
{
	clearAnimations();
}

void AnimationManager::updateNextFrame(const float& deltaTime)
{
	timer->beginTimedSection();

	activateAnimationsInPlayQueue();

	for (ActiveAnimation& activeAnimation : activeAnimations)
	{
		updateActiveAnimationFrame(activeAnimation, deltaTime);
	}

	timer->endTimedSection();
}

void AnimationManager::queueAnimationPlay(Message* message)
{
	PlayAnimationMessage* playMessage = static_cast<PlayAnimationMessage*>(message);

	QueuedAnimation newAnimation(playMessage->gameObjectID, playMessage->animationParams);
	newAnimation.transitionParams = playMessage->transition;
	animationsToAddtoPlayQueue.push_back(newAnimation);
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

	for (const ActiveAnimation& activeAnimation : activeAnimations)
	{
		if (activeAnimation.animation->hasGameObjectIdMatchOnly(id))
		{
			activeAnimation.animation->readAnimationState(animationStates);
			break;
		}
	}
}

void AnimationManager::updateActiveAnimationFrame(ActiveAnimation & activeAnimation, const float deltaTime)
{
	activeAnimation.animation->incrementTimer((double)deltaTime);

	bool transitioned = false;
	if (activeAnimation.animation->finishedPlaying())
	{
		if (activeAnimation.hasTransition())
		{
			QueuedAnimation transitionalAnimation(activeAnimation.transition.gameObjectId, activeAnimation.transition.params);
			animationsToAddtoPlayQueue.push_back(transitionalAnimation);
			transitioned = true;
		}
	}

	if (activeAnimation.animation->meshIsOnScreen() && !transitioned)
	{
		activeAnimation.animation->updateAnimationTransformState();
	}
}

void AnimationManager::activateAnimationsInPlayQueue()
{
	if (!animationsToAddtoPlayQueue.empty())
	{
		for (QueuedAnimation& queuedAnimation : animationsToAddtoPlayQueue)
		{
			const size_t gameObjectId = Hash{}(queuedAnimation.gameObjectId);
			const size_t animationId = Hash{}(queuedAnimation.params.animationName);
			const bool alreadyPlaying = removeActiveAnimation(gameObjectId, animationId);

			if (!alreadyPlaying)
			{
				beginPlayingAnimation(gameObjectId, animationId, queuedAnimation.params, 
					QueuedAnimation(queuedAnimation.gameObjectId, queuedAnimation.transitionParams));
			}
		}

		animationsToAddtoPlayQueue.clear();
	}
}

bool AnimationManager::removeActiveAnimation(const size_t& gameObjectId, const size_t& animationId)
{
	std::vector<ActiveAnimation>::iterator animationIterator;
	for (animationIterator = activeAnimations.begin(); animationIterator != activeAnimations.end(); ++animationIterator)
	{
		if ((*animationIterator).animation->hasGameObjectIdMatchOnly(gameObjectId))
		{
			if (!(*animationIterator).animation->hasAnimationIdMatchOnly(animationId))
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

void AnimationManager::beginPlayingAnimation(const size_t& gameObjectId, const size_t& animationId, const AnimationParams& params, const QueuedAnimation& transition)
{
	for (Animation* animation : animations)
	{
		if (animation->hasIdMatch(gameObjectId, animationId))
		{
			activeAnimations.push_back(ActiveAnimation(animation, transition));
			animation->reset();
			animation->setDurationToLerpFromPreviousAniamtion(params.lerpToTime);
			animation->setLooping(params.loop);
			break;
		}
	}
}
