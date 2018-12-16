#include "AnimationManager.h"

#include "Animation.h"
#include "AnimationComponents.h"
#include "../Utilities/GameTimer.h"
#include "../Utility/Camera.h"
#include "../../Communication/Messages/PlayAnimationMessage.h"
#include "../../Resource Management/Database/Database.h"
#include "../../Input/Devices/Keyboard.h"

using namespace std::placeholders;

const float DEGREES90_TO_RADIANS = 1.5708f;
vector<aiMatrix4x4> AnimationManager::emptyTransforms = vector<aiMatrix4x4>(100, aiMatrix4x4());

AnimationManager::AnimationManager(Database* database, Keyboard* keyboard, Camera* camera)
	: Subsystem("AnimationManager")
{
	this->database = database;
	this->keyboard = keyboard;
	this->camera = camera;
	drawActiveSkeletons = false;

	incomingMessages = MessageProcessor(std::vector<MessageType> { MessageType::PLAY_ANIMATION, MessageType::MOVE_CAMERA_RELATIVE_TO_GAMEOBJECT },
		DeliverySystem::getPostman()->getDeliveryPoint("AnimationManager"));

	incomingMessages.addActionToExecuteOnMessage(MessageType::PLAY_ANIMATION,
		std::bind(&AnimationManager::queueAnimationPlay, this, _1));
	incomingMessages.addActionToExecuteOnMessage(MessageType::MOVE_CAMERA_RELATIVE_TO_GAMEOBJECT, 
		std::bind(&AnimationManager::moveCameraWithAnimatedGameObject, this, _1));
}

AnimationManager::~AnimationManager()
{
	clearAnimations();
}

void AnimationManager::updateNextFrame(const float& deltaTime)
{
	timer->beginTimedSection();

	toggleDrawingSkeletonIfKeyTriggered();
	activateAnimationsInPlayQueue();

	std::vector<ActiveAnimation>::iterator animationIterator;
	for (animationIterator = activeAnimations.begin(); animationIterator != activeAnimations.end();)
	{
		drawActiveSkeleton(animationIterator);
		updateActiveAnimationFrame(animationIterator, deltaTime);
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

void AnimationManager::moveCameraWithAnimatedGameObject(Message* message)
{
	MoveCameraRelativeToGameObjectMessage* movementMessage = static_cast<MoveCameraRelativeToGameObjectMessage*>(message);
	GameObject* gameObject = static_cast<GameObject*>(database->getTable("GameObjects")->getResource(movementMessage->resourceName));
	const size_t id = Hash{}(movementMessage->resourceName);

	NCLMatrix4 currentAnimTransform;
	for (const ActiveAnimation& activeAnimation : activeAnimations)
	{
		if (activeAnimation.animation->hasGameObjectIdMatchOnly(id))
		{
			currentAnimTransform = activeAnimation.animation->getCurrentTransformOfSceneNodeTransformerNode(activeAnimation.gameObjectTransformSpecifier.nodeName);
			break;
		}
	}

	camera->setPosition(gameObject->getSceneNode()->GetTransform().getPositionVector() + currentAnimTransform.getPositionVector() + movementMessage->translation);
	camera->setPitch(movementMessage->pitch);
	camera->setYaw(movementMessage->yaw);
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
	bool foundAnimation = false;

	for (const ActiveAnimation& activeAnimation : activeAnimations)
	{
		if (activeAnimation.animation->hasGameObjectIdMatchOnly(id))
		{
			activeAnimation.animation->readAnimationState(animationStates);
			foundAnimation = true;
			break;
		}
	}

	if (!foundAnimation)
	{
		animationStates = emptyTransforms;
	}
}

void AnimationManager::updateActiveAnimationFrame(std::vector<ActiveAnimation>::iterator& animationIterator, const float deltaTime)
{
	animationIterator->animation->incrementTimer((double)deltaTime);

	if (animationIterator->animation->finishedPlaying())
	{
		TransformGameObject(animationIterator);
		completeActiveAnimation(animationIterator);
	}
	else if (animationIterator->animation->meshIsOnScreen())
	{
		animationIterator->animation->updateAnimationTransformState();
		++animationIterator;
	}
}

void AnimationManager::TransformGameObject(std::vector<ActiveAnimation>::iterator& animationIterator)
{
	if (animationIterator->gameObjectTransformSpecifier.nodeName != "")
	{
		animationIterator->animation->updateSceneNodeTransformFromNode(animationIterator->gameObjectTransformSpecifier);

		if (!animationIterator->animation->isLooping())
		{
			animationIterator->gameObjectTransformSpecifier.nodeName = "";
		}
	}
}

void AnimationManager::completeActiveAnimation(std::vector<ActiveAnimation>::iterator& animationIterator)
{
	if (animationIterator->hasTransition())
	{
		QueuedAnimation transitionalAnimation(animationIterator->transition.gameObjectId, animationIterator->transition.params);
		transitionalAnimation.params.transformBlocker = animationIterator->transition.params.transformBlocker;
		animationsToAddtoPlayQueue.push_back(transitionalAnimation);
		++animationIterator;
	}
	else if (!animationIterator->animation->isLooping())
	{
		animationIterator->animation->reset();
		animationIterator = activeAnimations.erase(animationIterator);
	}
	else
	{
		animationIterator->animation->reset();
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

void AnimationManager::beginPlayingAnimation(const size_t& gameObjectId, const size_t& animationId, 
	const AnimationParams& params, const QueuedAnimation& transition)
{
	for (Animation* animation : animations)
	{
		if (animation->hasIdMatch(gameObjectId, animationId))
		{
			activeAnimations.push_back(ActiveAnimation(animation, transition, params.gameObjectTransformSpecifier));
			animation->reset();
			animation->setDurationToLerpFromPreviousAniamtion(params.lerpToTime);
			animation->setLooping(params.loop);
			animation->blockTransformationForNode(params.transformBlocker.nodeName, params.transformBlocker.blockedComponents);

			break;
		}
	}
}

void AnimationManager::toggleDrawingSkeletonIfKeyTriggered()
{
	if (keyboard->keyTriggered(KEYBOARD_F9))
	{
		drawActiveSkeletons = !drawActiveSkeletons;
	}
}

void AnimationManager::drawActiveSkeleton(std::vector<ActiveAnimation>::iterator& animationIterator)
{
	if (drawActiveSkeletons)
	{
		Resource* parentResource = database->getTable("GameObjects")->getResource(animationIterator->animation->getOwningGameObjectName());
		GameObject* parentGameObject = static_cast<GameObject*>(parentResource);

		aiMatrix4x4 parentTransform;
		aiMatrix4x4 rotation;

		parentGameObject->getSceneNode()->GetWorldTransform().toASSIMPaiMatrix(parentTransform);
		aiMatrix4x4::RotationX(DEGREES90_TO_RADIANS, rotation);

		parentTransform = parentTransform * rotation;
		animationIterator->animation->debugDrawSkeleton(parentTransform);
	}
}
