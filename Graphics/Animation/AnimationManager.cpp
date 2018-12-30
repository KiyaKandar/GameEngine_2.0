#include "AnimationManager.h"

#include "Animation.h"
#include "AnimationComponents.h"
#include "../Utilities/GameTimer.h"
#include "../Utility/Camera.h"
#include "../../Communication/Messages/PlayAnimationMessage.h"
#include "../../Resource Management/Database/Database.h"

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
		DeliverySystem::GetPostman()->GetDeliveryPoint("AnimationManager"));

	incomingMessages.AddActionToExecuteOnMessage(MessageType::PLAY_ANIMATION,
		std::bind(&AnimationManager::QueueAnimationPlay, this, _1));
	incomingMessages.AddActionToExecuteOnMessage(MessageType::MOVE_CAMERA_RELATIVE_TO_GAMEOBJECT, 
		std::bind(&AnimationManager::MoveCameraWithAnimatedGameObject, this, _1));

	f9Listener = SinglePressKeyListener(KEYBOARD_F9, keyboard);
}

AnimationManager::~AnimationManager()
{
	ClearAnimations();
}

void AnimationManager::UpdateNextFrame(const float& deltaTime)
{
	timer->beginTimedSection();

	ToggleDrawingSkeletonIfKeyTriggered();
	ActivateAnimationsInPlayQueue();

	std::vector<ActiveAnimation>::iterator animationIterator;
	for (animationIterator = activeAnimations.begin(); animationIterator != activeAnimations.end();)
	{
		DrawActiveSkeleton(animationIterator);
		UpdateActiveAnimationFrame(animationIterator, deltaTime);
	}

	timer->endTimedSection();
}

void AnimationManager::QueueAnimationPlay(Message* message)
{
	PlayAnimationMessage* playMessage = static_cast<PlayAnimationMessage*>(message);

	QueuedAnimation newAnimation(playMessage->gameObjectID, playMessage->animationParams);
	newAnimation.transitionParams = playMessage->transition;
	animationsToAddtoPlayQueue.push_back(newAnimation);
}

void AnimationManager::MoveCameraWithAnimatedGameObject(Message* message)
{
	MoveCameraRelativeToGameObjectMessage* movementMessage = static_cast<MoveCameraRelativeToGameObjectMessage*>(message);
	GameObject* gameObject = static_cast<GameObject*>(database->GetTable("GameObjects")->GetResource(movementMessage->resourceName));
	const size_t id = Hash{}(movementMessage->resourceName);

	NCLMatrix4 currentAnimTransform;
	for (const ActiveAnimation& activeAnimation : activeAnimations)
	{
		if (activeAnimation.animation->HasGameObjectIdMatchOnly(id))
		{
			currentAnimTransform = activeAnimation.animation->GetCurrentTransformOfSceneNodeTransformerNode(activeAnimation.gameObjectTransformSpecifier.nodeName);
			break;
		}
	}

	camera->SetPosition(gameObject->GetSceneNode()->GetTransform().getPositionVector() + currentAnimTransform.getPositionVector() + movementMessage->translation);
	camera->SetPitch(movementMessage->pitch);
	camera->SetYaw(movementMessage->yaw);
}

void AnimationManager::AddAnimation(const std::string& animationName, const std::string& gameObjectId, Mesh* mesh, const aiAnimation* animation, 
	const aiNode* rootNode, const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo)
{
	animations.push_back(new Animation(animationName, gameObjectId, mesh, animation, rootNode, globalInverseTransform, initialBoneInfo));
}

void AnimationManager::ClearAnimations()
{
	for (Animation* animation : animations)
	{
		delete animation;
	}

	animations.clear();
	activeAnimations.clear();
	animationsToAddtoPlayQueue.clear();
}

void AnimationManager::ReadAnimationStateForSceneNode(const std::string& gameObjectId, std::vector<aiMatrix4x4>& animationStates) const
{
	const size_t id = Hash{}(gameObjectId);
	bool foundAnimation = false;

	for (const ActiveAnimation& activeAnimation : activeAnimations)
	{
		if (activeAnimation.animation->HasGameObjectIdMatchOnly(id))
		{
			activeAnimation.animation->ReadAnimationState(animationStates);
			foundAnimation = true;
			break;
		}
	}

	if (!foundAnimation)
	{
		animationStates = emptyTransforms;
	}
}

void AnimationManager::UpdateActiveAnimationFrame(std::vector<ActiveAnimation>::iterator& animationIterator, const float deltaTime)
{
	animationIterator->animation->IncrementTimer((double)deltaTime);

	if (animationIterator->animation->FinishedPlaying())
	{
		TransformGameObject(animationIterator);
		CompleteActiveAnimation(animationIterator);
	}
	else if (animationIterator->animation->MeshIsOnScreen())
	{
		animationIterator->animation->UpdateAnimationTransformState();
		++animationIterator;
	}
}

void AnimationManager::TransformGameObject(std::vector<ActiveAnimation>::iterator& animationIterator)
{
	if (animationIterator->gameObjectTransformSpecifier.nodeName != "")
	{
		animationIterator->animation->UpdateSceneNodeTransformFromNode(animationIterator->gameObjectTransformSpecifier);

		if (!animationIterator->animation->IsLooping())
		{
			animationIterator->gameObjectTransformSpecifier.nodeName = "";
		}
	}
}

void AnimationManager::CompleteActiveAnimation(std::vector<ActiveAnimation>::iterator& animationIterator)
{
	if (animationIterator->HasTransition())
	{
		QueuedAnimation transitionalAnimation(animationIterator->transition.gameObjectId, animationIterator->transition.params);
		transitionalAnimation.params.transformBlocker = animationIterator->transition.params.transformBlocker;
		animationsToAddtoPlayQueue.push_back(transitionalAnimation);
		++animationIterator;
	}
	else if (!animationIterator->animation->IsLooping())
	{
		animationIterator->animation->Reset();
		animationIterator = activeAnimations.erase(animationIterator);
	}
	else
	{
		animationIterator->animation->Reset();
	}
}

void AnimationManager::ActivateAnimationsInPlayQueue()
{
	if (!animationsToAddtoPlayQueue.empty())
	{
		for (QueuedAnimation& queuedAnimation : animationsToAddtoPlayQueue)
		{
			const size_t gameObjectId = Hash{}(queuedAnimation.gameObjectId);
			const size_t animationId = Hash{}(queuedAnimation.params.animationName);
			const bool alreadyPlaying = RemoveActiveAnimation(gameObjectId, animationId);

			if (!alreadyPlaying)
			{
				BeginPlayingAnimation(gameObjectId, animationId, queuedAnimation.params, 
					QueuedAnimation(queuedAnimation.gameObjectId, queuedAnimation.transitionParams));
			}
		}

		animationsToAddtoPlayQueue.clear();
	}
}

bool AnimationManager::RemoveActiveAnimation(const size_t& gameObjectId, const size_t& animationId)
{
	std::vector<ActiveAnimation>::iterator animationIterator;
	for (animationIterator = activeAnimations.begin(); animationIterator != activeAnimations.end(); ++animationIterator)
	{
		if ((*animationIterator).animation->HasGameObjectIdMatchOnly(gameObjectId))
		{
			if (!(*animationIterator).animation->HasAnimationIdMatchOnly(animationId))
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

void AnimationManager::BeginPlayingAnimation(const size_t& gameObjectId, const size_t& animationId, 
	const AnimationParams& params, const QueuedAnimation& transition)
{
	for (Animation* animation : animations)
	{
		if (animation->HasIdMatch(gameObjectId, animationId))
		{
			activeAnimations.push_back(ActiveAnimation(animation, transition, params.gameObjectTransformSpecifier));
			animation->Reset();
			animation->SetDurationToLerpFromPreviousAniamtion(params.lerpToTime);
			animation->SetLooping(params.loop);
			animation->SetBlockedSkeletonNodeTransforms(params.transformBlocker.nodeName, params.transformBlocker.blockedComponents);

			break;
		}
	}
}

void AnimationManager::ToggleDrawingSkeletonIfKeyTriggered()
{
	if (f9Listener.KeyPressed())
	{
		drawActiveSkeletons = !drawActiveSkeletons;
	}
}

void AnimationManager::DrawActiveSkeleton(std::vector<ActiveAnimation>::iterator& animationIterator)
{
	if (drawActiveSkeletons)
	{
		Resource* parentResource = database->GetTable("GameObjects")->GetResource(animationIterator->animation->GetOwningGameObjectName());
		GameObject* parentGameObject = static_cast<GameObject*>(parentResource);

		aiMatrix4x4 parentTransform;
		aiMatrix4x4 rotation;

		parentGameObject->GetSceneNode()->GetWorldTransform().toASSIMPaiMatrix(parentTransform);
		aiMatrix4x4::RotationX(DEGREES90_TO_RADIANS, rotation);

		parentTransform = parentTransform * rotation;
		animationIterator->animation->DebugDrawSkeleton(parentTransform);
	}
}
