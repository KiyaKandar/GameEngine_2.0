#pragma once
#include "PhysicsNode.h"
#include "SphereCollisionShape.h"
#include "CuboidCollisionShape.h"
#include "../Utilities/Maths/Matrix4.h"

enum class boundingShapes { Cuboid, Sphere };

struct BParea {
	std::vector<PhysicsNode*> nodesInArea;
	NCLVector3 minVal;
	NCLVector3 maxVal;
	int depthLvl = 0;
	BParea(NCLVector3 min, NCLVector3 max, int depth = 0) : minVal(min), maxVal(max), depthLvl(depth) {}
	bool operator==(const BParea& rhs) const { return (minVal == rhs.minVal && maxVal == rhs.maxVal) ? true : false; };
};


class BroadPhaseCulling
{
public:
	BroadPhaseCulling();
	~BroadPhaseCulling();

	void initBroadphase(NCLVector3 minVal, NCLVector3 maxVal);



	static bool SphereSphereCollision(const PhysicsNode* a, const PhysicsNode* b);

	static  const boundingShapes boundingBox = boundingShapes::Sphere;

	void sortNode(PhysicsNode* p);

	void clear();

	bool splitArea(BParea a);

	static bool intersect(NCLVector3 minVal, NCLVector3 maxVal, NCLVector3 spherePos, float radius);

	std::vector<BParea> getBpAreas() { return bpAreas; };

	void DebugDraw(std::vector<DebugLineMessage>& messages);

protected:
	int maxNodes = 10;
	int maxDepth = 4;

	std::vector<BParea> bpAreas;

};