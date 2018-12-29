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

	void drawSkeleton(const MeshNode& parentNode, const aiMatrix4x4& parentTransform);

private:
	bool readyToDrawSkeleton();
	void clearPreviousDraw();
	void prepareSkeletonBoneRendering(const MeshNode& parentNode, const aiMatrix4x4& parentTransform);

	void drawChildSkeletonBone(const MeshNode& parentNode, const aiMatrix4x4& parentTransform);

	void getJointPosition(aiVector3D& position, const aiMatrix4x4& jointTrainsform);
	void displayJointNode(const aiVector3D& position);
	void displayBoneLine(const aiVector3D& startPosition, const aiVector3D& endPosition);

	TrackedGroupMessageSender<DebugLineMessage> skeletonBoneMessageSender;
	std::vector<DebugLineMessage> skeletonBoneMessages;

	TrackedGroupMessageSender<DebugSphereMessage> skeletonJointMessageSender;
	std::vector<DebugSphereMessage> skeletonJointMessages;

	bool bufferMessages = false;
};

