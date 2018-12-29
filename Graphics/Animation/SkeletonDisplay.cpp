#include "SkeletonDisplay.h"

#include "Skeleton.h"

void SkeletonDisplay::drawSkeleton(const MeshNode& parentNode, const aiMatrix4x4& parentTransform)
{
	if (readyToDrawSkeleton())
	{
		clearPreviousDraw();
		prepareSkeletonBoneRendering(parentNode, parentTransform);

		skeletonBoneMessageSender.setMessageGroup(skeletonBoneMessages);
		skeletonJointMessageSender.setMessageGroup(skeletonJointMessages);

		skeletonBoneMessageSender.sendMessageGroup();
		skeletonJointMessageSender.sendMessageGroup();
	}
}

bool SkeletonDisplay::readyToDrawSkeleton()
{
	return skeletonBoneMessageSender.readyToSendNextMessageGroup() && skeletonJointMessageSender.readyToSendNextMessageGroup();
}

void SkeletonDisplay::clearPreviousDraw()
{
	skeletonBoneMessages.clear();
	skeletonJointMessages.clear();
}

void SkeletonDisplay::prepareSkeletonBoneRendering(const MeshNode& parentNode, const aiMatrix4x4& parentTransform)
{
	aiVector3D startPosition;
	getJointPosition(startPosition, parentTransform * parentNode.rawTransform);
	displayJointNode(startPosition);

	for (int i = 0; i < parentNode.node->mNumChildren; ++i)
	{
		aiVector3D endPosition;
		getJointPosition(endPosition, parentTransform * parentNode.children[i].rawTransform);
		displayBoneLine(startPosition, endPosition);
	}

	drawChildSkeletonBone(parentNode, parentTransform);
}

void SkeletonDisplay::drawChildSkeletonBone(const MeshNode& parentNode, const aiMatrix4x4& parentTransform)
{
	const unsigned int numChildren = parentNode.node->mNumChildren;

	if (numChildren > 0)
	{
		for (int i = 0; i < numChildren; ++i)
		{
			prepareSkeletonBoneRendering(parentNode.children[i], parentTransform);
		}
	}
	else
	{
		aiVector3D jointPosition;
		getJointPosition(jointPosition, parentTransform * parentNode.rawTransform);
		displayJointNode(jointPosition);
	}
}

void SkeletonDisplay::getJointPosition(aiVector3D& position, const aiMatrix4x4& jointTrainsform)
{
	jointTrainsform.DecomposeNoScaling(aiQuaternion(), position);
}

void SkeletonDisplay::displayJointNode(const aiVector3D& position)
{
	skeletonJointMessages.push_back(DebugSphereMessage("RenderingSystem", position, 0.5f, NCLVector3(1.0f, 0.0f, 0.0f)));
}

void SkeletonDisplay::displayBoneLine(const aiVector3D& startPosition, const aiVector3D& endPosition)
{
	skeletonBoneMessages.push_back(DebugLineMessage("RenderingSystem", startPosition, endPosition, NCLVector3(0.0f, 1.0f, 0.2f)));
}
