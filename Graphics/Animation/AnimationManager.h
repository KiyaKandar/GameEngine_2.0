#pragma once

#include "../Launch/Systems/Subsystem.h"
#include "AnimationService.h"

class Animation;
class Database;

struct QueuedAnimation;

class AnimationManager : public Subsystem, public AnimationService
{
public:
	AnimationManager(Database* database);
	~AnimationManager();

	void updateNextFrame(const float& deltaTime) override;

	void QueueAnimationPlay(Message* message);
	void addAnimation(const std::string& animationName, Mesh* mesh, const aiAnimation* animation, const aiNode* rootNode, const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo) override;
	void clearAnimations();
	
	void readAnimationStateForMesh(const std::string& meshName, std::vector<aiMatrix4x4>& animationStates) const override;

private:
	void ActivateAnimationsInPlayQueue();
	bool RemoveActiveMeshAnimation(const size_t& meshId, const size_t& animationId);
	void BeginPlayingAnimation(const size_t& meshId, const size_t& animationId, const double lerpToTime);

	std::vector<QueuedAnimation> animationsToAddtoPlayQueue;
	std::vector<Animation*> activeAnimations;
	std::vector<Animation*> animations;

	Database* database;
};

