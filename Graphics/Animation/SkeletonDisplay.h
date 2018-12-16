#pragma once

#include <matrix4x4.h>

struct MeshNode;

class SkeletonDisplay
{
public:
	static void drawSkeletonBone(const MeshNode& parentNode, const aiMatrix4x4& parentTransform);

private:
	static void drawChildSkeletonBone(const MeshNode& parentNode, const aiMatrix4x4& parentTransform);

	static void getJointPosition(aiVector3D& position, const aiMatrix4x4& jointTrainsform);
	static void displayJointNode(const aiVector3D& position);
	static void displayBoneLine(const aiVector3D& startPosition, const aiVector3D& endPosition);
};

