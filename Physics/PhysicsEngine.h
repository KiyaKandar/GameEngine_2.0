#pragma once

#include "../Launch/Systems/Subsystem.h"
#include "../Resource Management/Database/Database.h"

#include "PhysicsNode.h"
#include "Constraint.h"
#include "Manifold.h"
#include "BroadPhaseCulling.h"
#include <vector>
#include <unordered_set>

class OctreePartitioning;
class Keyboard;


//Number of jacobi iterations to apply in order to
// assure the constraints are solved. (Last tutorial)
#define SOLVER_ITERATIONS 50

struct CollisionPair
{
	PhysicsNode* pObjectA;
	PhysicsNode* pObjectB;
	bool operator==(const CollisionPair& rhs) const { return(pObjectA == rhs.pObjectA && pObjectB == rhs.pObjectB) ? true : false; };
};

namespace std {
	template <> struct hash<CollisionPair>
	{
		size_t operator()(const CollisionPair &x) const {
			return hash<float>()(x.pObjectA->getPosition().x) + hash<float>()(x.pObjectB->getPosition().y) + hash<float>()(x.pObjectA->getPosition().y) + hash<float>()(x.pObjectB->getPosition().y);
		}
	};
}


class PhysicsEngine : public Subsystem
{
public:
	
	PhysicsEngine(Database* database, Keyboard* keyboard);
	~PhysicsEngine();

	void addPhysicsObject(PhysicsNode* obj);
	void removePhysicsObject(PhysicsNode* obj);
	void removeAllPhysicsObjects();

	void addConstraint(Constraint* c) { constraints.push_back(c); }


	void updateNextFrame(const float& deltaTime) override;


	inline float getUpdateTimestep() const 
	{ 
		return updateTimestep; 
	}
	inline void setUpdateTimestep(float updateTimestep) 
	{ 
		updateTimestep = updateTimestep; 
	}

	inline const NCLVector3& getGravity() const 
	{ 
		return gravity; 
	}
	inline void setGravity(const NCLVector3& g) 
	{ 
		gravity = g; 
	}

	inline float getDampingFactor() const 
	{ 
		return dampingFactor; 
	}
	inline void  setDampingFactor(float d) 
	{ 
		dampingFactor = d; 
	}

	inline float getDeltaTime() const { 
		return updateTimestep; 
	}

	inline BroadPhaseCulling& getBPcull() { return BpOct; };

	inline int getNumCols() const { return broadphaseColPairs.size(); };

	void OctreeChanged(const NCLMatrix4 &matrix)
	{
		octreeChanged = true;
	}

	void InitialiseOctrees(int entityLimit);

private:
	void updatePhysics();

	void broadPhaseCollisions();

	void narrowPhaseCollisions();

	float		updateTimestep, updateRealTimeAccum;
	
	NCLVector3		gravity;
	float		dampingFactor;


	std::unordered_set<CollisionPair>  broadphaseColPairs;

	std::vector<PhysicsNode*>	physicsNodes;

	std::vector<Constraint*>	constraints;
	std::vector<Manifold*>		manifolds;

	BroadPhaseCulling BpOct;


	bool		octreeChanged = false;
	bool		octreeInitialised = false;
	//OctreePartitioning* octree;


	Database* database;

	Keyboard* keyboard;
	bool wireframeRendering = false;
};