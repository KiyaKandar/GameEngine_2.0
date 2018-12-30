#pragma once

#include "../Launch/Systems/Subsystem.h"
#include "../../Input/Devices/Keyboard.h"
#include "AnimationService.h"

class Animation;
class Database;
class Camera;

struct QueuedAnimation;
struct ActiveAnimation;
struct AnimationParams;

class AnimationManager : public Subsystem, public AnimationService
{
public:
	AnimationManager(Database* database, Keyboard* keyboard, Camera* camera);
	~AnimationManager();

	void UpdateNextFrame(const float& deltaTime) override;

	void QueueAnimationPlay(Message* message);
	void MoveCameraWithAnimatedGameObject(Message* message);

	void AddAnimation(const std::string& animationName, const std::string& gameObjectId, Mesh* mesh, const aiAnimation* animation,
		const aiNode* rootNode, const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo) override;
	void ClearAnimations();
	
	void ReadAnimationStateForSceneNode(const std::string& gameObjectId, std::vector<aiMatrix4x4>& animationStates) const override;

private:
	void UpdateActiveAnimationFrame(std::vector<ActiveAnimation>::iterator& animationIterator, const float deltaTime);
	void TransformGameObject(std::vector<ActiveAnimation>::iterator& animationIterator);
	void CompleteActiveAnimation(std::vector<ActiveAnimation>::iterator& animationIterator);

	void ActivateAnimationsInPlayQueue();
	bool RemoveActiveAnimation(const size_t& gameObjectId, const size_t& animationId);
	void BeginPlayingAnimation(const size_t& gameObjectId, const size_t& animationId, 
		const AnimationParams& params, const QueuedAnimation& transition);

	void ToggleDrawingSkeletonIfKeyTriggered();
	void DrawActiveSkeleton(std::vector<ActiveAnimation>::iterator& animationIterator);

	std::vector<QueuedAnimation> animationsToAddtoPlayQueue;
	std::vector<ActiveAnimation> activeAnimations;
	std::vector<Animation*> animations;
	std::vector<size_t> activeAnimationsToStop;

	Database* database;
	Keyboard* keyboard;
	SinglePressKeyListener f9Listener;
	Camera* camera;

	bool drawActiveSkeletons;

	static std::vector<aiMatrix4x4> emptyTransforms;
};

