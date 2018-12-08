#pragma once

#include "../Meshes/Mesh.h"
#include "Animation.h"

#include <anim.h>
#include <matrix4x4.h>
#include <scene.h>

class AnimationService
{
public:
	virtual void addAnimation(Mesh* mesh, const aiAnimation* animation, const aiNode* rootNode, const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo) = 0;
	virtual void readAnimationStateForMesh(const std::string& meshName, std::vector<aiMatrix4x4>& animationStates) const = 0;
};