/*
	The glue that brings all our physics dreams together. This class is provided
	pretty much as is, as most of the sub-systems are tied in to this already
	with a bit of debugging code to visualise various parts of the physics engine.

	The general runtime consists of:
		- Update(float dt)
		  - UpdatePhysics()
			 - Broadphase Collision Detection
			   Quickly identifies possible collisions in between objects, hopefully
			   with world space partitioning systems like Octrees.. but currently just
			   builds a list colliding all objects with all other objects. (Hopefully you
			   can find some free time to fix this =] )

			 - Narrowphase Collision Detection
			   Takes the list provided by the broadphase collision detection and 
			   accurately collides all objects, building a collision manifold as
			   required. (Tutorial 4/5)

			 - Solve Constraints & Collisions
			   Solves all velocity constraints in the physics system, these include
			   both Collision Constraints (Tutorial 5,6) and misc world constraints
			   like distance constraints (Tutorial 3)

			 - Update Physics Objects
			   Moves all physics objects through time, updating positions/rotations
			   etc. each iteration (Tutorial 2)

*/

#pragma once

#include "../Launch/Systems/Subsystem.h"
#include "../Resource Management/Database/Database.h"
#include "../Communication/MessageSenders/TrackedGroupMessageSender.h"

#include "PhysicsNode.h"
#include "Constraint.h"
#include "Manifold.h"
#include <vector>
#include <unordered_set>

//#include "GPUCloth.h"

class GPUCloth;
class OctreePartitioning;
class Keyboard;

//Number of jacobi iterations to apply in order to
// assure the constraints are solved. (Last tutorial)
#define SOLVER_ITERATIONS 60

//Just saves including windows.h for the sake of defining true/false
#ifndef FALSE
	#define FALSE	0
	#define TRUE	1
#endif

struct CollisionPair	//Forms the output of the broadphase collision detection
{
	PhysicsNode* pObjectA;
	PhysicsNode* pObjectB;
};

class PhysicsEngine : public Subsystem
{
public:
	PhysicsEngine(Database* database, Keyboard* keyboard);
	~PhysicsEngine();

	void addPhysicsObject(PhysicsNode* obj);
	void RemovePhysicsObject(PhysicsNode* obj);
	void RemoveAllPhysicsObjects();

	void AddConstraint(Constraint* c) { constraints.push_back(c); }
	
	void updateNextFrame(const float& deltaTime) override;

	void InitialiseOctrees(int entityLimit);
	void OctreeChanged(const NCLMatrix4 &matrix)
	{
		octreeChanged = true;
	}

protected:
	//The actual time-independant update function
	void UpdatePhysics();

	void BroadPhaseCollisions();
	void NarrowPhaseCollisions();

protected:
	bool		octreeChanged = false;
	bool		octreeInitialised = false;
	float		updateTimestep, updateRealTimeAccum;

	Database* database;
	Keyboard* keyboard;

	std::vector<CollisionPair>  broadphaseColPairs;

	std::vector<PhysicsNode*>	physicsNodes;

	std::vector<Constraint*>	constraints;		// Misc constraints applying to one or more physics objects e.g our DistanceConstraint
	std::vector<Manifold*>		manifolds;			// Contact constraints between pairs of objects

	OctreePartitioning* octree;

	int debugRenderMode = 0;
	TrackedGroupMessageSender<DebugLineMessage> cubeDrawMessageSender;
	TrackedGroupMessageSender<DebugSphereMessage> sphereDrawMessageSender;
};