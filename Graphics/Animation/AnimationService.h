#pragma once

#include "../Meshes/Mesh.h"
#include "AnimationComponents.h"

#include <anim.h>
#include <matrix4x4.h>
#include <scene.h>

class AnimationService
{
public:
	virtual void AddAnimation(const std::string& animationName, const std::string& gameObjectId, Mesh* mesh, const aiAnimation* animation, const aiNode* rootNode,
		const aiMatrix4x4& globalInverseTransform, std::vector<BoneInfo>* initialBoneInfo) = 0;
	virtual void ReadAnimationStateForSceneNode(const std::string& meshName, std::vector<aiMatrix4x4>& animationStates) const = 0;
};