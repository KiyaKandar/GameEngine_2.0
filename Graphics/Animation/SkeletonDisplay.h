#pragma once

#include "../../Communication/MessageSenders/TrackedGroupMessageSender.h"
#include "../../Communication/Messages/DebugLineMessage.h"
#include "../../Communication/Messages/DebugSphereMessage.h"

#include <matrix4x4.h>

struct MeshNode;

class SkeletonDisplay
{
public:
	SkeletonDisplay() {}
	~SkeletonDisplay() {}

	void DrawSkeleton(const MeshNode& parentNode, const aiMatrix4x4& parentTransform);

private:
	bool ReadyToDrawSkeleton();
	void ClearPreviousDraw();
	void PrepareSkeletonBoneRendering(const MeshNode& parentNode, const aiMatrix4x4& parentTransform);

	void DrawChildSkeletonBone(const MeshNode& parentNode, const aiMatrix4x4& parentTransform);

	void GetJointPosition(aiVector3D& position, const aiMatrix4x4& jointTrainsform);
	void DisplayJointNode(const aiVector3D& position);
	void DisplayBoneLine(const aiVector3D& startPosition, const aiVector3D& endPosition);

	TrackedGroupMessageSender<DebugLineMessage> skeletonBoneMessageSender;
	std::vector<DebugLineMessage> skeletonBoneMessages;

	TrackedGroupMessageSender<DebugSphereMessage> skeletonJointMessageSender;
	std::vector<DebugSphereMessage> skeletonJointMessages;

	bool bufferMessages = false;
};

