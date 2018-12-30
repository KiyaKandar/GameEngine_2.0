#include "SkeletonDisplay.h"

#include "Skeleton.h"

void SkeletonDisplay::DrawSkeleton(const MeshNode& parentNode, const aiMatrix4x4& parentTransform)
{
	if (ReadyToDrawSkeleton())
	{
		ClearPreviousDraw();
		PrepareSkeletonBoneRendering(parentNode, parentTransform);

		skeletonBoneMessageSender.SetMessageGroup(skeletonBoneMessages);
		skeletonJointMessageSender.SetMessageGroup(skeletonJointMessages);

		skeletonBoneMessageSender.SendMessageGroup();
		skeletonJointMessageSender.SendMessageGroup();
	}
}

bool SkeletonDisplay::ReadyToDrawSkeleton()
{
	return skeletonBoneMessageSender.ReadyToSendNextMessageGroup() && skeletonJointMessageSender.ReadyToSendNextMessageGroup();
}

void SkeletonDisplay::ClearPreviousDraw()
{
	skeletonBoneMessages.clear();
	skeletonJointMessages.clear();
}

void SkeletonDisplay::PrepareSkeletonBoneRendering(const MeshNode& parentNode, const aiMatrix4x4& parentTransform)
{
	aiVector3D startPosition;
	GetJointPosition(startPosition, parentTransform * parentNode.rawTransform);
	DisplayJointNode(startPosition);

	for (int i = 0; i < parentNode.node->mNumChildren; ++i)
	{
		aiVector3D endPosition;
		GetJointPosition(endPosition, parentTransform * parentNode.children[i].rawTransform);
		DisplayBoneLine(startPosition, endPosition);
	}

	DrawChildSkeletonBone(parentNode, parentTransform);
}

void SkeletonDisplay::DrawChildSkeletonBone(const MeshNode& parentNode, const aiMatrix4x4& parentTransform)
{
	const unsigned int numChildren = parentNode.node->mNumChildren;

	if (numChildren > 0)
	{
		for (int i = 0; i < numChildren; ++i)
		{
			PrepareSkeletonBoneRendering(parentNode.children[i], parentTransform);
		}
	}
	else
	{
		aiVector3D jointPosition;
		GetJointPosition(jointPosition, parentTransform * parentNode.rawTransform);
		DisplayJointNode(jointPosition);
	}
}

void SkeletonDisplay::GetJointPosition(aiVector3D& position, const aiMatrix4x4& jointTrainsform)
{
	jointTrainsform.DecomposeNoScaling(aiQuaternion(), position);
}

void SkeletonDisplay::DisplayJointNode(const aiVector3D& position)
{
	skeletonJointMessages.push_back(DebugSphereMessage("RenderingSystem", position, 0.5f, NCLVector3(1.0f, 0.0f, 0.0f)));
}

void SkeletonDisplay::DisplayBoneLine(const aiVector3D& startPosition, const aiVector3D& endPosition)
{
	skeletonBoneMessages.push_back(DebugLineMessage("RenderingSystem", startPosition, endPosition, NCLVector3(0.0f, 1.0f, 0.2f)));
}
