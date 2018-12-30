#include "OctreePartitioning.h"

OctreePartitioning::OctreePartitioning(vector<PhysicsNode*> nodes, 
	NCLVector3 worldPartitionHalfDimensions, NCLVector3 offset)
{
	this->nodes = nodes;
	this->offset = offset;

	//Remove nodes that will not be colliding
	for (auto it = this->nodes.begin(); it != this->nodes.end();)
	{
		if (!(*it)->collidable)
		{
			it = this->nodes.erase(it);
		}
		else
		{
			++it;
		}
	}

	this->worldPartitionHalfDimensions = worldPartitionHalfDimensions;

	CreateInitialOctants();
}

OctreePartitioning::~OctreePartitioning()
{
}

void OctreePartitioning::BuildInitialTree()
{
	vector<PhysicsNode*> nodesToConsider = nodes;

	for (Partition& partition : baseOctreePartitions)
	{
		PartitionNodes(partition, nodesToConsider);
	}
}

void OctreePartitioning::UpdateTree()
{
	//Find out which nodes require updating.
	for (Partition& partition : baseOctreePartitions)
	{
		CheckIfPartitionAndChildrenHaveChanged(partition);
	}

	//Remove those nodes from the every partition they
	//are a member of.
	for (Partition& partition : baseOctreePartitions)
	{
		DeleteUpdatedNodes(partition);
	}
	
	//Update the flags so this does not happen next frame.
	for each (PhysicsNode* node in nodesToUpdate)
	{
		node->toDeleteInOctree = false;
	}

	//Reinsert into the tree.
	//Must be inserted with the knowledge that the tree could
	//have branches with leaf nodes.
	for (Partition& partition : baseOctreePartitions)
	{
		for each (PhysicsNode* node in nodesToUpdate)
		{	
			PartitionNodeIntoExistingTree(partition, node);
		}
	}

	//Periodically clear an octree that is completely empty
	if (clearCounter == 30)
	{
		ClearAllEmptyOctants();
		clearCounter = 0;
	}

	++clearCounter;

	nodesToUpdate.clear();
}

void OctreePartitioning::PartitionNodes(Partition& parent, vector<PhysicsNode*>& parentNodes)
{
	for (PhysicsNode* node : parentNodes)
	{
		for each (CollisionShape* shape in node->collisionShapes)
		{
			if (PartitionUtility::CollidesWithPartition(parent, shape))
			{
				parent.containedNodes.push_back(node);
				break;
			}
		}
	}

	if (parent.containedNodes.size() > ENTITY_PER_PARTITION_THRESHOLD)
	{
		PartitionUtility::SplitPartition(parent);

		//Remove them from this partition and allocate the nodes
		//to child partitions.
		for (Partition& childPartition : parent.childPartitions)
		{
			PartitionNodes(childPartition, parent.containedNodes);
		}

		parent.containedNodes.clear();
	}
}

void OctreePartitioning::PartitionNodeIntoExistingTree(Partition& parent, PhysicsNode* node)
{
	if (parent.childPartitions.size() == 0)
	{
		for each (CollisionShape* shape in node->collisionShapes)
		{
			if (PartitionUtility::CollidesWithPartition(parent, shape))
			{
				parent.containedNodes.push_back(node);
				break;
			}
		}

		if (parent.containedNodes.size() > ENTITY_PER_PARTITION_THRESHOLD)
		{
			PartitionUtility::SplitPartition(parent);

			for (Partition& childPartition : parent.childPartitions)
			{
				PartitionNodes(childPartition, parent.containedNodes);
			}

			parent.containedNodes.clear();
		}
	}
	else
	{
		//Check child partitions for collisions with node.
		for (Partition& child : parent.childPartitions)
		{
			for each (CollisionShape* shape in node->collisionShapes)
			{
				if (PartitionUtility::CollidesWithPartition(child, shape))
				{
					PartitionNodeIntoChild(child, node);
					break;
				}
			}
		}
	}
}

void OctreePartitioning::PartitionNodeIntoChild(Partition& child, PhysicsNode* node)
{
	//Check if this node contains child partitions.
	PartitionNodeIntoExistingTree(child, node);

	if (child.containedNodes.size() > ENTITY_PER_PARTITION_THRESHOLD)
	{
		PartitionUtility::SplitPartition(child);

		for (Partition& childPartition : child.childPartitions)
		{
			PartitionNodes(childPartition, child.containedNodes);
		}

		child.containedNodes.clear();
	}
}

vector<CollisionPair> OctreePartitioning::GetAllCollisionPairs()
{
	if (!collisionPairsBuilt)
	{
		pairs.clear();

		for (Partition& partition : baseOctreePartitions)
		{
			GetCollisionPairsForPartition(partition);
		}

		collisionPairsBuilt = true;
	}

	return pairs;
}

void OctreePartitioning::GetCollisionPairsForPartition(Partition& partition)
{
	if (partition.containedNodes.size() > 1)
	{
		PartitionUtility::StoreCollisionPairs(pairs, partition);
	}

	for (Partition& childPartition : partition.childPartitions)
	{
		GetCollisionPairsForPartition(childPartition);
	}
}

void OctreePartitioning:: CreateInitialOctants()
{
	float bottomRowYCoord = worldPartitionHalfDimensions.y / 4 + offset.y;
	float topRowYCoord = worldPartitionHalfDimensions.y / 2 + bottomRowYCoord + offset.y;

	PartitionUtility::CreateQuadTreeRow(baseOctreePartitions, worldPartitionHalfDimensions, NCLVector3(offset.x, bottomRowYCoord, offset.z));
	PartitionUtility::CreateQuadTreeRow(baseOctreePartitions, worldPartitionHalfDimensions, NCLVector3(offset.x, topRowYCoord, offset.z));
}

void OctreePartitioning::UpdateNodesPartition(PhysicsNode* node)
{
	vector<PhysicsNode*> movedNode = { node };

	for (Partition& partition : baseOctreePartitions)
	{
		PartitionNodes(partition, movedNode);
	}
}

void OctreePartitioning::DeleteUpdatedNodes(Partition & partition)
{
	for (auto it = partition.containedNodes.begin(); it != partition.containedNodes.end();)
	{
		if ((*it)->toDeleteInOctree)
		{
			it = partition.containedNodes.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (Partition& child : partition.childPartitions)
	{
		DeleteUpdatedNodes(child);
	}
}

void OctreePartitioning::CheckIfPartitionAndChildrenHaveChanged(Partition& partition)
{
	for (PhysicsNode* node : partition.containedNodes)
	{
		if (node->movedSinceLastBroadPhase)
		{
			nodesToUpdate.push_back(node);
			node->movedSinceLastBroadPhase = false;
			node->toDeleteInOctree = true;
			collisionPairsBuilt = false;
		}
	}

	for (Partition& child : partition.childPartitions)
	{
		CheckIfPartitionAndChildrenHaveChanged(child);
	}
}

void OctreePartitioning::ClearAllEmptyOctants()
{
	for (Partition& partition : baseOctreePartitions)
	{
		ClearEmptyOctants(partition);
	}
}

void OctreePartitioning::ClearEmptyOctants(Partition& partition)
{
	int emptyPartitions = 0;
	for (Partition& child : partition.childPartitions)
	{
		if (child.childPartitions.size() != 0) //child is leaf node
		{
			ClearEmptyOctants(child);
		}
		else if (child.containedNodes.size() == 0) //check if all leaf nodes on this level are empty
		{
			++emptyPartitions;
		}
	}

	if (emptyPartitions == 8)
	{
		partition.childPartitions.clear();
	}
}

void OctreePartitioning::DrawWireFrameOctrees(std::vector<DebugLineMessage>& lineMessages)
{
	for (const Partition& partition : baseOctreePartitions)
	{
		PartitionUtility::DrawPartitionAndChildren(partition, lineMessages);
	}
}