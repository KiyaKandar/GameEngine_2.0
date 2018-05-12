#include "BroadPhaseCulling.h"



BroadPhaseCulling::BroadPhaseCulling()
{
	maxNodes = 10;
}


BroadPhaseCulling::~BroadPhaseCulling()
{
}

void BroadPhaseCulling::initBroadphase(NCLVector3 minVal, NCLVector3 maxVal)
{
	NCLVector3 areaSize = (maxVal - minVal) / 2;
	for (int x = 0; x < 2; ++x) {
		for (int y = 0; y < 2; ++y) {
			for (int z = 0; z < 2; ++z) {
				NCLVector3 minVal(minVal.x + (areaSize.x * x), minVal.y + (areaSize.y*y), minVal.z + (areaSize.z*z));
				NCLVector3 maxVal(maxVal.x - (areaSize.x*(1 - x)), maxVal.y - (areaSize.y*(1 - y)), maxVal.z - (areaSize.z*(1 - z)));
				bpAreas.push_back(BParea(minVal, maxVal));
			}
		}
	}
}

bool BroadPhaseCulling::SphereSphereCollision(const PhysicsNode* a, const PhysicsNode* b)
{
	if (boundingBox == boundingShapes::Sphere) {
		SphereCollisionShape* aSphr = static_cast<SphereCollisionShape*>(a->getBroadPhaseShape());
		SphereCollisionShape* bSphr = static_cast<SphereCollisionShape*>(b->getBroadPhaseShape());

		if (aSphr && bSphr) {
			if ((b->getPosition() - a->getPosition()).length() <= aSphr->getRadius() + bSphr->getRadius()) return true;
		}
	}
	return false;
}

void BroadPhaseCulling::sortNode(PhysicsNode * p)
{
	//collision detection with axis aligned boundingBox of Octrees
	if (p->getBroadPhaseShape()) {

		float radius = static_cast<SphereCollisionShape*>(p->getBroadPhaseShape())->getRadius();
		NCLVector3 centre = p->getPosition();
		//use iterator instead 
		for (size_t i = 0; i < bpAreas.size(); ++i) {
			if (intersect(bpAreas[i].minVal, bpAreas[i].maxVal, centre, radius)) {
				std::vector<PhysicsNode*>::iterator index = std::find(bpAreas[i].nodesInArea.begin(), bpAreas[i].nodesInArea.end(), p);
				if (index != bpAreas[i].nodesInArea.end()) bpAreas[i].nodesInArea.erase(index);
				bpAreas[i].nodesInArea.push_back(p);
				splitArea((bpAreas.at(i)));
			}
			else {
				//check if area had it previously and remove it?
				std::vector<PhysicsNode*>::iterator index = std::find(bpAreas[i].nodesInArea.begin(), bpAreas[i].nodesInArea.end(), p);
				if (index != bpAreas[i].nodesInArea.end()) bpAreas[i].nodesInArea.erase(index);
			}
		}
	}

}

void BroadPhaseCulling::clear()
{
	bpAreas.erase(bpAreas.begin(), bpAreas.end());
	bpAreas.clear();
}

bool BroadPhaseCulling::splitArea(BParea a)
{
	if (a.nodesInArea.size() >= (size_t)maxNodes && a.depthLvl < maxDepth) {
		NCLVector3 areaSize = (a.maxVal - a.minVal) / 2;
		for (int x = 0; x < 2; ++x) {
			for (int y = 0; y < 2; ++y) {
				for (int z = 0; z < 2; ++z) {
					NCLVector3 minVal(a.minVal.x + (areaSize.x * x), a.minVal.y + (areaSize.y*y), a.minVal.z + (areaSize.z*z));
					NCLVector3 maxVal(a.maxVal.x - (areaSize.x*(1 - x)), a.maxVal.y - (areaSize.y*(1 - y)), a.maxVal.z - (areaSize.z*(1 - z)));
					bpAreas.push_back(BParea(minVal, maxVal, a.depthLvl + 1));

				}
			}
		}
		std::vector<BParea>::iterator index = std::find(bpAreas.begin(), bpAreas.end(), a);
		//sort nodes again that was in this partition

		BParea a = *(index);
		bpAreas.erase(index);
		for (PhysicsNode* p : a.nodesInArea) { sortNode(p); };
		return true;
	}
	else return false;
}



bool BroadPhaseCulling::intersect(NCLVector3 minVal, NCLVector3 maxVal, NCLVector3 spherePos, float radius)
{
	if (spherePos.x - radius < maxVal.x && spherePos.x + radius > minVal.x) {
		if (spherePos.y - radius < maxVal.y && spherePos.y + radius > minVal.y) {
			if (spherePos.z - radius < maxVal.z && spherePos.z + radius > minVal.z) {
				return true;
			}
		}
	}
	return false;
}

void BroadPhaseCulling::DebugDraw()
{
	for (BParea cube : bpAreas) {

		NCLVector3 scale = (cube.maxVal - cube.minVal) / 2;

		NCLMatrix4 transform = NCLMatrix4::translation(cube.minVal + ((cube.maxVal - cube.minVal) / 2)) * NCLMatrix4::scale(NCLVector3(scale.x, scale.y, scale.z));
		int numNodes = cube.nodesInArea.size();
		float full = numNodes / (float)maxNodes;
		CuboidCollisionShape::getcubeHull().DebugDraw(transform);
	}
}