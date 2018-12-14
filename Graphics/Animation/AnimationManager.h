#pragma once

#include "../Launch/Systems/Subsystem.h"
#include "AnimationService.h"

class Animation;
class Database;

struct QueuedAnimation;
struct ActiveAnimation;
struct AnimationParams;

class AnimationManager : public Subsystem, public AnimationService
{
public:
	AnimationManager(Database* database);
	~AnimationManager();

	void updateNextFrame(const float& deltaTime) override;

	void queueAnimationPlay(Message* message);
	void addAnimation(const std::string& animationName, const std::string& gameObjectId, Mesh* mesh, const aiAnimation* animation,
		const aiNode* rootNode, const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo) override;
	void clearAnimations();
	
	void readAnimationStateForSceneNode(const std::string& gameObjectId, std::vector<aiMatrix4x4>& animationStates) const override;

private:
	void updateActiveAnimationFrame(std::vector<ActiveAnimation>::iterator& animationIterator, const float deltaTime);
	void CompleteActiveAnimation(std::vector<ActiveAnimation>::iterator& animationIterator);

	void activateAnimationsInPlayQueue();
	bool removeActiveAnimation(const size_t& gameObjectId, const size_t& animationId);
	void beginPlayingAnimation(const size_t& gameObjectId, const size_t& animationId, 
		const AnimationParams& params, const QueuedAnimation& transition);

	std::vector<QueuedAnimation> animationsToAddtoPlayQueue;
	std::vector<ActiveAnimation> activeAnimations;
	std::vector<Animation*> animations;
	std::vector<size_t> activeAnimationsToStop;

	Database* database;
};

