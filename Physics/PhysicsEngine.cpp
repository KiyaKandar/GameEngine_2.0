#include "PhysicsEngine.h"

#include "CollisionDetectionSAT.h"
#include "OctreePartitioning.h"
#include <omp.h>
#include <algorithm>

#include "GPUCloth.h"

#include "../Communication/Messages/ApplyForceMessage.h"
#include "../Communication/Messages/CollisionMessage.h"
#include "../Utilities/GameTimer.h"
#include "../Communication/Messages/AbsoluteTransformMessage.h"
#include "../Input/Devices/Keyboard.h"

PhysicsEngine::PhysicsEngine(Database* database, Keyboard* keyboard) : Subsystem("Physics")
{
	this->database = database;
	this->keyboard = keyboard;
	octree = nullptr;

	std::vector<MessageType> types = { MessageType::TEXT, MessageType::PLAYER_INPUT, MessageType::RELATIVE_TRANSFORM,
		MessageType::APPLY_FORCE, MessageType::APPLY_IMPULSE, MessageType::UPDATE_POSITION, MessageType::ABSOLUTE_TRANSFORM,
		MessageType::MOVE_GAMEOBJECT, MessageType::SCALE_GAMEOBJECT, MessageType::ROTATE_GAMEOBJECT, MessageType::TOGGLE_GAMEOBJECT };

	incomingMessages = MessageProcessor(types, DeliverySystem::GetPostman()->GetDeliveryPoint("Physics"));

	incomingMessages.AddActionToExecuteOnMessage(MessageType::TOGGLE_GAMEOBJECT, [database = database](Message* message)
	{
		ToggleGameObjectMessage* toggleMessage = static_cast<ToggleGameObjectMessage*>(message);

		GameObject* gameObject = static_cast<GameObject*>(
			database->GetTable("GameObjects")->GetResource(toggleMessage->gameObjectID));

		gameObject->GetPhysicsNode()->SetEnabled(toggleMessage->isEnabled);
		gameObject->SetEnabled(toggleMessage->isEnabled);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::TEXT, [database = database, this](Message* message)
	{
		TextMessage* textMessage = static_cast<TextMessage*>(message);

		istringstream iss(textMessage->text);
		vector<string> tokens{ istream_iterator<string>{iss},
			std::istream_iterator<string>{} };

		if (tokens[0] == "addphysicsnode")
		{
			GameObject* gameObject = static_cast<GameObject*>(
				database->GetTable("GameObjects")->GetResource(tokens[1]));

			AddPhysicsObject(gameObject->GetPhysicsNode());
		}
		else if (tokens[0] == "removephysicsnode")
		{
			for (auto physicsNodeiterator = physicsNodes.begin(); physicsNodeiterator != physicsNodes.end(); ++physicsNodeiterator)
			{
				if ((*physicsNodeiterator)->GetParent()->GetName() == tokens[1])
				{
					physicsNodes.erase(physicsNodeiterator);
					break;
				}
			}
		}
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::ABSOLUTE_TRANSFORM, [database = database](Message* message)
	{
		AbsoluteTransformMessage* translationMessage = static_cast<AbsoluteTransformMessage*>(message);
		GameObject* gameObject = static_cast<GameObject*>(
			database->GetTable("GameObjects")->GetResource(translationMessage->resourceName));

		gameObject->GetPhysicsNode()->SetPosition(translationMessage->transform.GetPositionVector());
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::MOVE_GAMEOBJECT, [database = database](Message* message)
	{
		MoveGameObjectMessage* moveMessage = static_cast<MoveGameObjectMessage*>(message);

		GameObject* gameObject = static_cast<GameObject*>(
			database->GetTable("GameObjects")->GetResource(moveMessage->gameObjectID));

		gameObject->GetPhysicsNode()->SetPosition(moveMessage->position);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::SCALE_GAMEOBJECT, [database = database](Message* message)
	{
		ScaleGameObjectMessage* scaleMessage = static_cast<ScaleGameObjectMessage*>(message);

		GameObject* gameObject = static_cast<GameObject*>(
			database->GetTable("GameObjects")->GetResource(scaleMessage->gameObjectID));

		gameObject->GetPhysicsNode()->GetCollisionShape()->SetScale(scaleMessage->scale, gameObject->GetPhysicsNode()->GetInverseMass());
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::ROTATE_GAMEOBJECT, [database = database](Message* message)
	{
		RotateGameObjectMessage* rotateMessage = static_cast<RotateGameObjectMessage*>(message);

		GameObject* gameObject = static_cast<GameObject*>(
			database->GetTable("GameObjects")->GetResource(rotateMessage->gameObjectID));

		if (rotateMessage->relative)
		{
			gameObject->GetPhysicsNode()->SetOrientation(gameObject->GetPhysicsNode()->GetOrientation() *
				Quaternion::AxisAngleToQuaterion(NCLVector3(rotateMessage->rotation.x, rotateMessage->rotation.y, rotateMessage->rotation.z), rotateMessage->rotation.w));
		}
		else
		{
			gameObject->GetPhysicsNode()->SetOrientation(
				Quaternion::AxisAngleToQuaterion(NCLVector3(rotateMessage->rotation.x, rotateMessage->rotation.y, rotateMessage->rotation.z), rotateMessage->rotation.w));
		}
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::APPLY_FORCE, [database/*, this*/](Message* message)
	{
		ApplyForceMessage* applyForceMessage = static_cast<ApplyForceMessage*>(message);

		GameObject* gObj = static_cast<GameObject*>(database->GetTable("GameObjects")->GetResource(applyForceMessage->gameObjectID));

		NCLVector3 force = applyForceMessage->force /* * this->getDeltaTime() * 1000*/;

		if (applyForceMessage->isRandom)
		{
			if (applyForceMessage->xmin != applyForceMessage->xmax)
			{
				force.x = VectorBuilder::GetRandomVectorComponent(applyForceMessage->xmin, applyForceMessage->xmax) * 50.0f;
				//These * 50.0f are needed because currently rand() doesn't give good results for large ranges. 
				//So a smaller range is needed when choosing random min and max values for vectors, which should then be scaled to the appropriate value
				//Get rid of them though as any random force component will now be scaled and this isn't good!
			}
			if (applyForceMessage->ymin != applyForceMessage->ymax)
			{
				force.y = VectorBuilder::GetRandomVectorComponent(applyForceMessage->ymin, applyForceMessage->ymax) * 50.0f;
			}
			if (applyForceMessage->zmin != applyForceMessage->zmax)
			{
				force.z = VectorBuilder::GetRandomVectorComponent(applyForceMessage->zmin, applyForceMessage->zmax) * 50.0f;
			}
		}

		gObj->GetPhysicsNode()->SetAppliedForce(force);
	});

	incomingMessages.AddActionToExecuteOnMessage(MessageType::APPLY_IMPULSE, [database](Message* message)
	{
		ApplyImpulseMessage* applyImpulseMessage = static_cast<ApplyImpulseMessage*>(message);

		GameObject* gObj = static_cast<GameObject*>(database->GetTable("GameObjects")->GetResource(applyImpulseMessage->gameObjectID));

		NCLVector3 impulse = applyImpulseMessage->impulse;

		if (applyImpulseMessage->isRandom)
		{
			if (applyImpulseMessage->xmin != applyImpulseMessage->xmax)
			{
				impulse.x = VectorBuilder::GetRandomVectorComponent(applyImpulseMessage->xmin, applyImpulseMessage->xmax);
			}
			if (applyImpulseMessage->ymin != applyImpulseMessage->ymax)
			{
				impulse.y = VectorBuilder::GetRandomVectorComponent(applyImpulseMessage->ymin, applyImpulseMessage->ymax);
			}
			if (applyImpulseMessage->zmin != applyImpulseMessage->zmax)
			{
				impulse.z = VectorBuilder::GetRandomVectorComponent(applyImpulseMessage->zmin, applyImpulseMessage->zmax);
			}
		}
		gObj->GetPhysicsNode()->ApplyImpulse(impulse);
	});



	incomingMessages.AddActionToExecuteOnMessage(MessageType::UPDATE_POSITION, [database/*, &dt*/](Message* message)
	{
		UpdatePositionMessage* positionMessage = static_cast<UpdatePositionMessage*>(message);

		GameObject* gObj = static_cast<GameObject*>(database->GetTable("GameObjects")->GetResource(positionMessage->gameObjectID));

		gObj->GetPhysicsNode()->SetPosition((positionMessage->position)/**dt*/);
	});

	updateTimestep = 1.0f / 60.f;
	updateRealTimeAccum = 0.0f;

	timer->AddChildTimer("Broadphase");
	timer->AddChildTimer("Narrowphase");
	timer->AddChildTimer("Solver");
	timer->AddChildTimer("Integrate Position");
	timer->AddChildTimer("Integrate Velocity");
}

PhysicsEngine::~PhysicsEngine()
{
	RemoveAllPhysicsObjects();
	delete octree;
}

void PhysicsEngine::AddPhysicsObject(PhysicsNode* obj)
{
	if (octreeInitialised)
	{
		octreeChanged = true;
		obj->movedSinceLastBroadPhase = true;
		octree->AddNode(obj);
	}

	physicsNodes.push_back(obj);

	obj->SetOnCollisionCallback([](PhysicsNode* this_obj, PhysicsNode* colliding_obj, CollisionData collisionData)
	{
		if (this_obj->transmitCollision)
		{
			if (!this_obj->hasTransmittedCollision)
			{
				if (this_obj->collisionMessageSender.ReadyToSendNextMessage())
				{
					this_obj->collisionMessageSender.SetMessage(CollisionMessage("Gameplay", collisionData,
						this_obj->GetParent()->GetName(), colliding_obj->GetParent()->GetName()));
					this_obj->collisionMessageSender.SendTrackedMessage();

					if (!this_obj->multipleTransmitions)
					{
						this_obj->hasTransmittedCollision = true;
					}
				}
			}

			return true;
		}
		return true;
	});

	obj->SetOnUpdateCallback(std::bind(
		&PhysicsEngine::OctreeChanged,
		this,
		std::placeholders::_1));
}

void PhysicsEngine::RemovePhysicsObject(PhysicsNode* obj)
{
	//Lookup the object in question
	auto found_loc = std::find(physicsNodes.begin(), physicsNodes.end(), obj);

	//If found, remove it from the list
	if (found_loc != physicsNodes.end())
	{
		physicsNodes.erase(found_loc);
	}
}

void PhysicsEngine::RemoveAllPhysicsObjects()
{
	//Delete and remove all constraints/collision manifolds
	for (Constraint* c : constraints)
	{
		delete c;
	}
	constraints.clear();

	for (Manifold* m : manifolds)
	{
		delete m;
	}
	manifolds.clear();


	//Delete and remove all physics objects
	// - we also need to inform the (possibly) associated game-object
	//   that the physics object no longer exists
	for (PhysicsNode* obj : physicsNodes)
	{
		if (obj->GetParent()) obj->GetParent()->SetPhysicsNode(nullptr);
		delete obj;
	}

	physicsNodes.clear();
}


void PhysicsEngine::UpdateNextFrame(const float& deltaTime)
{
	static const int maxUpdatesPerFrame = 5;
	updateRealTimeAccum += deltaTime * 0.001f;
	const bool doUpdate = updateRealTimeAccum >= updateTimestep;

	if (doUpdate)
	{
		timer->BeginTimedSection();
	}


	for (int i = 0; (updateRealTimeAccum >= updateTimestep) && i < maxUpdatesPerFrame; ++i)
	{
		updateRealTimeAccum -= updateTimestep;

		UpdatePhysics();
	}

	if (updateRealTimeAccum >= updateTimestep)
	{
		updateRealTimeAccum = 0.0f;
	}

	for (PhysicsNode* physicsNode : physicsNodes)
	{
		physicsNode->hasTransmittedCollision = false;
	}

	if (keyboard->KeyTriggered(KEYBOARD_F8))
	{
		debugRenderMode++;

		if (debugRenderMode > 3)
		{
			debugRenderMode = 0;
		}
	}

	if (debugRenderMode > 0)
	{
		if (cubeDrawMessageSender.ReadyToSendNextMessageGroup() && sphereDrawMessageSender.ReadyToSendNextMessageGroup())
		{
			std::vector<DebugLineMessage> cubeDrawMessages;
			std::vector<DebugSphereMessage> sphereDrawMessages;

			if (debugRenderMode == 1)
			{
				octree->DrawWireFrameOctrees(cubeDrawMessages);
			}
			else if (debugRenderMode == 2)
			{
				for (PhysicsNode* node : physicsNodes)
				{
					node->GetCollisionShape()->DebugDraw(cubeDrawMessages, sphereDrawMessages);
				}
			}
			else if (debugRenderMode == 3)
			{
				for (Manifold* m : manifolds)
				{
					m->DebugDraw(cubeDrawMessages, sphereDrawMessages);
				}
			}

			cubeDrawMessageSender.SetMessageGroup(cubeDrawMessages);
			sphereDrawMessageSender.SetMessageGroup(sphereDrawMessages);

			cubeDrawMessageSender.SendMessageGroup();
			sphereDrawMessageSender.SendMessageGroup();
		}
	}

	if (doUpdate)
	{
		timer->EndTimedSection();
	}
}


void PhysicsEngine::UpdatePhysics()
{
	for (Manifold* m : manifolds)
	{
		delete m;
	}
	manifolds.clear();

	timer->BeginChildTimedSection("Broadphase");
	BroadPhaseCollisions();
	timer->EndChildTimedSection("Broadphase");

	timer->BeginChildTimedSection("Narrowphase");
	NarrowPhaseCollisions();

	std::random_shuffle(manifolds.begin(), manifolds.end());
	std::random_shuffle(constraints.begin(), constraints.end());

	for (Manifold* m : manifolds) m->PreSolverStep(updateTimestep);
	for (Constraint* c : constraints) c->PreSolverStep(updateTimestep);
	timer->EndChildTimedSection("Narrowphase");

	timer->BeginChildTimedSection("Integrate Velocity");
	for (PhysicsNode* obj : physicsNodes)
	{
		if (obj->GetEnabled())
		{
			obj->IntegrateForVelocity(updateTimestep);
		}
	}
	timer->EndChildTimedSection("Integrate Velocity");

	timer->BeginChildTimedSection("Solver");
	for (size_t i = 0; i < SOLVER_ITERATIONS; ++i)
	{
		for (Manifold* m : manifolds) m->ApplyImpulse();
		for (Constraint* c : constraints) c->ApplyImpulse();
	}
	timer->EndChildTimedSection("Solver");

	timer->BeginChildTimedSection("Integrate Position");
	for (PhysicsNode* obj : physicsNodes) 
	{
		if (obj->GetEnabled())
		{
			obj->IntegrateForPosition(updateTimestep);
		}
	}
	timer->EndChildTimedSection("Integrate Position");
}

void PhysicsEngine::BroadPhaseCollisions()
{
	PhysicsNode *pnodeA, *pnodeB;
	//	The broadphase needs to build a list of all potentially colliding objects in the world,
	//	which then get accurately assesed in narrowphase. If this is too coarse then the system slows down with
	//	the complexity of narrowphase collision checking, if this is too fine then collisions may be missed.

	if (physicsNodes.size() > 0)
	{
		if (octreeChanged)
		{
			octree->UpdateTree();
			octreeChanged = false;
			broadphaseColPairs = octree->GetAllCollisionPairs();
		}
	}
}


void PhysicsEngine::NarrowPhaseCollisions()
{
	if (broadphaseColPairs.size() > 0)
	{
		//Collision data to pass between detection and manifold generation stages.
		CollisionData colData;				

		//Collision Detection Algorithm to use
		CollisionDetectionSAT colDetect;	

		// Iterate over all possible collision pairs and perform accurate collision detection
		for (size_t i = 0; i < broadphaseColPairs.size(); ++i)
		{
			CollisionPair& cp = broadphaseColPairs[i];

			for each (CollisionShape* shapeA in cp.pObjectA->collisionShapes)
			{
				for each (CollisionShape* shapeB in cp.pObjectB->collisionShapes)
				{
					colDetect.BeginNewPair(
						cp.pObjectA,
						cp.pObjectB,
						shapeA,
						shapeB);

					// Detects if the objects are colliding
					if (colDetect.AreColliding(&colData))
					{
						//Note: As at the end of tutorial 4 we have very little to do, this is a bit messier
						//      than it should be. We now fire oncollision events for the two objects so they
						//      can handle AI and also optionally draw the collision normals to see roughly
						//      where and how the objects are colliding.

						//Draw collision data to the window if requested
						// - Have to do this here as colData is only temporary. 
						//if (debugDrawFlags & DEBUGDRAW_FLAGS_COLLISIONNORMALS)
						//{
						//	NCLDebug::DrawPointNDT(colData._pointOnPlane, 0.1f, Vector4(0.5f, 0.5f, 1.0f, 1.0f));
						//	NCLDebug::DrawThickLineNDT(colData._pointOnPlane, colData._pointOnPlane - colData._normal * colData._penetration, 0.05f, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
						//	//NCLDebug::DrawThickLineNDT(cp.pObjectA->GetPosition(), cp.pObjectB->GetPosition(), 0.1f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
						//}

						//Check to see if any of the objects have a OnCollision callback that dont want the objects to physically collide
						bool okA = cp.pObjectA->FireOnCollisionEvent(cp.pObjectA, cp.pObjectB, colData);
						bool okB = cp.pObjectB->FireOnCollisionEvent(cp.pObjectB, cp.pObjectA, colData);

						if (okA && okB)
						{
							/* TUTORIAL 5 CODE */
							Manifold* manifold = new Manifold;

							manifold->Initiate(cp.pObjectA, cp.pObjectB);
							colDetect.GenContactPoints(manifold);

							if (manifold->contactPoints.size() > 0)
							{
								manifolds.push_back(manifold);
							}
							else
							{
								delete manifold;
							}
						}
					}
				}
			}
		}

	}
}

void PhysicsEngine::InitialiseOctrees(int entityLimit)
{
	if (octree)
	{
		delete octree;
	}

	octree = new OctreePartitioning(physicsNodes, NCLVector3(300, 300, 300), NCLVector3(0, 0 , 0));
	octree->ENTITY_PER_PARTITION_THRESHOLD = entityLimit;

	if(physicsNodes.size() > 0)
	{
		octree->BuildInitialTree();
	}

	octreeChanged = false;
	octreeInitialised = true;

	for each (PhysicsNode* node in physicsNodes)
	{
		node->movedSinceLastBroadPhase = false;
	}

	broadphaseColPairs = octree->GetAllCollisionPairs();
}
