#pragma once

#include "../Launch/Systems/Subsystem.h"
#include "AnimationService.h"

class Animation;

class AnimationManager : public Subsystem, public AnimationService
{
public:
	AnimationManager();
	~AnimationManager();

	void updateNextFrame(const float& deltaTime) override;

	void addAnimation(Mesh* mesh, const aiAnimation* animation, const aiNode* rootNode, const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo) override;
	void clearAnimations();
	
	void readAnimationStateForMesh(const std::string& meshName, std::vector<aiMatrix4x4>& animationStates) const override;

private:
	std::vector<Animation*> animations;
};

