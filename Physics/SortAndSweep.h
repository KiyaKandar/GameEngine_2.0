#pragma once

#include "../Utilities/Maths/Vector3.h"
#include "PhysicsNode.h"
#include "PhysicsEngine.h"
#include "AABBUtility.h"

#include <algorithm>
#include <vector>

const NCLVector3 X_AXIS = NCLVector3(1, 0, 0);

class SortAndSweep
{
public:

	static void PerformSortAndSweep(std::vector<CollisionPair>& pairs, std::vector<PhysicsNode*>& nodes)
	{
		SortEntitiesAlongXAxis(nodes);

		for (unsigned x = 0; x < nodes.size(); ++x)
		{
			if (nodes[x]->getEnabled())
			{
				float maxA = GetMaxXCoordOfNode(nodes[x]);

				for (unsigned y = x + 1; y < nodes.size(); ++y)
				{
					if (nodes[y]->getEnabled())
					{
						if (!(nodes[x]->getIsStatic() && nodes[y]->getIsStatic()))
						{
							float minB = GetMinXCoordOfNode(nodes[y]);

							//Are they possibly colliding?
							if (maxA > minB)
							{
								//Yes, get Narrow phase to check this pair properly...
								CollisionPair pair;
								pair.pObjectA = nodes[x];
								pair.pObjectB = nodes[y];

								pairs.push_back(pair);
							}
							else
							{
								break;
							}
						}
					}
				}
			}
		}
	}

	static void SortEntitiesAlongXAxis(std::vector<PhysicsNode*>& nodes)
	{
		std::sort(nodes.begin(), nodes.end(), CompareBoxes);
	}

	static bool CompareBoxes(PhysicsNode* a, PhysicsNode* b)
	{
		float minA = GetMinXCoordOfNode(a);
		float minB = GetMinXCoordOfNode(b);

		return minA < minB;
	}

	static float GetMinXCoordOfNode(PhysicsNode* node)
	{
		float minXCoord = FLT_MAX;
		NCLVector3 currentMinX;
		NCLVector3 currentMaxX;

		for each (CollisionShape* shape in node->collisionShapes)
		{
			shape->GetMinMaxVertexOnAxis(X_AXIS, currentMinX, currentMaxX);
			minXCoord = min(currentMinX.x, minXCoord);
		}

		return minXCoord;
	}

	static float GetMaxXCoordOfNode(PhysicsNode* node)
	{
		float maxXCoord = FLT_MIN;
		NCLVector3 currentMinX;
		NCLVector3 currentMaxX;

		for each (CollisionShape* shape in node->collisionShapes)
		{
			shape->GetMinMaxVertexOnAxis(X_AXIS, currentMinX, currentMaxX);
			maxXCoord = max(currentMaxX.x, maxXCoord);
		}

		return maxXCoord;
	}
};