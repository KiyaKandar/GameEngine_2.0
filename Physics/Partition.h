#pragma once

#include <vector>

#include "../Utilities/Maths/Vector3.h"
#include "PhysicsNode.h"
#include "AABBUtility.h"
#include "PhysicsEngine.h"
#include "SortAndSweep.h"

using namespace std;

const NCLVector3 WORLD_AXIS = NCLVector3(1, 1, 1);

struct Partition
{
	NCLVector3 centrePosition;
	NCLVector3 halfDimensions;
	NCLVector3 minPoint;
	NCLVector3 maxPoint;

	vector<Partition> childPartitions;
	vector<PhysicsNode*> containedNodes;

	Partition(NCLVector3 centrePosition, NCLVector3 halfDimensions)
	{
		this->centrePosition = centrePosition;
		this->halfDimensions = halfDimensions;

		minPoint = centrePosition - (halfDimensions);
		maxPoint = centrePosition + (halfDimensions);
	}

	Partition() = default;
};

class PartitionUtility 
{
public:
	static bool CollidesWithPartition(Partition& parent, CollisionShape* shape)
	{
		AABB nodeBox;
		shape->GetMinMaxVertexOnAxis(WORLD_AXIS, nodeBox.min, nodeBox.max);

		AABB partitionBox;
		partitionBox.min = parent.minPoint;
		partitionBox.max = parent.maxPoint;

		return BoxesIntersect(nodeBox, partitionBox);
	}

	static void SplitPartition(Partition& parent)
	{
		NCLVector3 quarterDimensions = parent.halfDimensions * 0.5f;

		float bottomRowYCoord = quarterDimensions.y *0.5f;
		float topRowYCoord = quarterDimensions.y + bottomRowYCoord;

		CreateQuadTreeRow(parent.childPartitions, parent.halfDimensions,
			NCLVector3(parent.centrePosition.x, (parent.centrePosition.y - quarterDimensions.y), parent.centrePosition.z));

		CreateQuadTreeRow(parent.childPartitions, parent.halfDimensions,
			NCLVector3(parent.centrePosition.x, (parent.centrePosition.y + quarterDimensions.y), parent.centrePosition.z));
	}

	static void StoreCollisionPairs(vector<CollisionPair>& pairs, Partition& partition)
	{
		SortAndSweep::PerformSortAndSweep(pairs, partition.containedNodes);
	}

	static void CreateQuadTreeRow(vector<Partition>& partitionsToAddTo,
		NCLVector3 halfDimensions, NCLVector3 basePosition)
	{
		NCLVector3 quarterDimensions = halfDimensions * 0.5;

		partitionsToAddTo.push_back(Partition(
			NCLVector3(-quarterDimensions.x, 0, -quarterDimensions.z) + basePosition,
			quarterDimensions));

		partitionsToAddTo.push_back(Partition(
			NCLVector3(-quarterDimensions.x, 0, quarterDimensions.z) + basePosition,
			quarterDimensions));

		partitionsToAddTo.push_back(Partition(
			NCLVector3(quarterDimensions.x, 0, -quarterDimensions.z) + basePosition,
			quarterDimensions));

		partitionsToAddTo.push_back(Partition(
			NCLVector3(quarterDimensions.x, 0, quarterDimensions.z) + basePosition,
			quarterDimensions));
	}

	static void DrawPartitionAndChildren(const Partition& partition, std::vector<DebugLineMessage>& lineMessages)
	{
		DrawPartition(partition, lineMessages);

		for (const Partition& child : partition.childPartitions)
		{
			DrawPartitionAndChildren(child, lineMessages);
		}
	}

	static void DrawPartition(const Partition& partition, std::vector<DebugLineMessage>& lineMessages)
	{
		NCLVector3 colour = NCLVector3(1.0, 0.0f, 0.0f);

		if (partition.containedNodes.size() == 0)
		{
			colour = NCLVector3(0.0, 0.0f, 1.0f);
		}

		lineMessages.push_back(DebugLineMessage("RenderingSystem",
			partition.minPoint, NCLVector3(partition.minPoint.x, partition.maxPoint.y, partition.minPoint.z),
			colour));
		lineMessages.push_back(DebugLineMessage("RenderingSystem",
			NCLVector3(partition.maxPoint.x, partition.minPoint.y, partition.maxPoint.z), partition.maxPoint,
			colour));

		lineMessages.push_back(DebugLineMessage("RenderingSystem",
			NCLVector3(partition.minPoint.x, partition.minPoint.y, partition.maxPoint.z), 
			NCLVector3(partition.minPoint.x, partition.maxPoint.y, partition.maxPoint.z),
			colour));
		lineMessages.push_back(DebugLineMessage("RenderingSystem",
			NCLVector3(partition.maxPoint.x, partition.minPoint.y, partition.minPoint.z),
			NCLVector3(partition.maxPoint.x, partition.maxPoint.y, partition.minPoint.z),
			colour));

		lineMessages.push_back(DebugLineMessage("RenderingSystem",
			partition.minPoint, NCLVector3(partition.maxPoint.x, partition.minPoint.y, partition.minPoint.z),
			colour));
		lineMessages.push_back(DebugLineMessage("RenderingSystem",
			NCLVector3(partition.minPoint.x, partition.maxPoint.y, partition.minPoint.z), 
			NCLVector3(partition.maxPoint.x, partition.maxPoint.y, partition.minPoint.z),
			colour));

		lineMessages.push_back(DebugLineMessage("RenderingSystem",
			partition.maxPoint, NCLVector3(partition.minPoint.x, partition.maxPoint.y, partition.maxPoint.z),
			colour));
		lineMessages.push_back(DebugLineMessage("RenderingSystem",
			NCLVector3(partition.maxPoint.x, partition.minPoint.y, partition.maxPoint.z), 
			NCLVector3(partition.minPoint.x, partition.minPoint.y, partition.maxPoint.z),
			colour));

		lineMessages.push_back(DebugLineMessage("RenderingSystem",
			partition.minPoint, NCLVector3(partition.minPoint.x, partition.minPoint.y, partition.maxPoint.z),
			colour));
		lineMessages.push_back(DebugLineMessage("RenderingSystem",
			NCLVector3(partition.minPoint.x, partition.maxPoint.y, partition.minPoint.z), 
			NCLVector3(partition.minPoint.x, partition.maxPoint.y, partition.maxPoint.z),
			colour));

		lineMessages.push_back(DebugLineMessage("RenderingSystem",
			NCLVector3(partition.maxPoint.x, partition.minPoint.y, partition.minPoint.z), 
			NCLVector3(partition.maxPoint.x, partition.minPoint.y, partition.maxPoint.z),
			colour));
		lineMessages.push_back(DebugLineMessage("RenderingSystem",
			NCLVector3(partition.maxPoint.x, partition.maxPoint.y, partition.minPoint.z), partition.maxPoint,
			colour));
	}
};