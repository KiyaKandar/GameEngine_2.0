#include "SkeletonDisplay.h"

#include "Skeleton.h"
#include "../../Communication/DeliverySystem.h"

void SkeletonDisplay::drawSkeletonBone(const MeshNode& parentNode, const aiMatrix4x4& parentTransform)
{
	aiVector3D startPosition;
	SkeletonDisplay::getJointPosition(startPosition, parentTransform * parentNode.rawTransform);
	SkeletonDisplay::displayJointNode(startPosition);

	for (int i = 0; i < parentNode.node->mNumChildren; ++i)
	{
		aiVector3D endPosition;
		SkeletonDisplay::getJointPosition(endPosition, parentTransform * parentNode.children[i].rawTransform);
		SkeletonDisplay::displayBoneLine(startPosition, endPosition);
	}

	SkeletonDisplay::drawChildSkeletonBone(parentNode, parentTransform);
}

void SkeletonDisplay::drawChildSkeletonBone(const MeshNode& parentNode, const aiMatrix4x4& parentTransform)
{
	const unsigned int numChildren = parentNode.node->mNumChildren;

	if (numChildren > 0)
	{
		for (int i = 0; i < numChildren; ++i)
		{
			SkeletonDisplay::drawSkeletonBone(parentNode.children[i], parentTransform);
		}
	}
	else
	{
		aiVector3D jointPosition;
		SkeletonDisplay::getJointPosition(jointPosition, parentTransform * parentNode.rawTransform);
		SkeletonDisplay::displayJointNode(jointPosition);
	}
}

void SkeletonDisplay::getJointPosition(aiVector3D& position, const aiMatrix4x4& jointTrainsform)
{
	jointTrainsform.DecomposeNoScaling(aiQuaternion(), position);
}

void SkeletonDisplay::displayJointNode(const aiVector3D& position)
{
	DebugSphereMessage jointMessage("RenderingSystem", position, 0.5f, NCLVector3(1.0f, 0.0f, 0.0f));
	DeliverySystem::getPostman()->insertMessage(jointMessage);
}

void SkeletonDisplay::displayBoneLine(const aiVector3D & startPosition, const aiVector3D & endPosition)
{
	DebugLineMessage boneLineMessage("RenderingSystem", startPosition, endPosition, NCLVector3(0.0f, 1.0f, 0.2f));
	DeliverySystem::getPostman()->insertMessage(boneLineMessage);
}
