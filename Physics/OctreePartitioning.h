#pragma once

#include "Partition.h"
#include "PhysicsEngine.h"

class OctreePartitioning
{
public:
	OctreePartitioning(vector<PhysicsNode*> nodes, 
		NCLVector3 worldPartitionHalfDimensions, NCLVector3 offset);
	~OctreePartitioning();

	void BuildInitialTree();
	void UpdateTree();
	vector<CollisionPair> GetAllCollisionPairs();

	inline void AddNode(PhysicsNode* newNode)
	{
		nodes.push_back(newNode);

		vector<PhysicsNode*> newNodes = { newNode };
		for (Partition& partition : baseOctreePartitions)
		{
			for each (PhysicsNode* node in newNodes)
			{
				PartitionNodeIntoExistingTree(partition, node);
			}
		}
	}

	void DrawWireFrameOctrees();

	int ENTITY_PER_PARTITION_THRESHOLD = 10;
private:
	void GetCollisionPairsForPartition(Partition& partition);
	void PartitionNodes(Partition& partition, vector<PhysicsNode*>& nodes);
	void PartitionNodeIntoExistingTree(Partition& parent, PhysicsNode* nodes);
	void PartitionNodeIntoChild(Partition& child, PhysicsNode* node);
	void CreateInitialOctants();
	void UpdateNodesPartition(PhysicsNode* node);
	void DeleteUpdatedNodes(Partition& partition);

	void CheckIfPartitionAndChildrenHaveChanged(Partition& partition);

	void ClearAllEmptyOctants();
	void ClearEmptyOctants(Partition& partition);

	vector<Partition> baseOctreePartitions;
	vector<PhysicsNode*> nodes;
	NCLVector3 worldPartitionHalfDimensions;
	NCLVector3 offset;

	bool collisionPairsBuilt = false;
	vector<CollisionPair> pairs;
	vector<PhysicsNode*> nodesToUpdate;

	int clearCounter = 0;
};

